#include "Include.h"
// ------------------------------------ //
#ifndef LEVIATHAN_CONNECTIONINFO
#include "ConnectionInfo.h"
#endif
#include "Utility/Iterators/WstringIterator.h"
#include "SFML/Network/IpAddress.hpp"
#include "SFML/Network/Packet.hpp"
#include "NetworkHandler.h"
using namespace Leviathan;
// ------------------------------------ //
DLLEXPORT Leviathan::ConnectionInfo::ConnectionInfo(shared_ptr<wstring> hostname) : HostName(hostname), AddressGot(false), LastUsedID(-1), 
	LastSentConfirmID(-1), MaxAckReduntancy(1), MyLastSentReceived(-1)
{
	// We need to split the port number from the address //
	WstringIterator itr(hostname.get(), false);

	auto result = itr.GetUntilNextCharacterOrAll(L':');

	HostName = shared_ptr<wstring>(result.release());

	// We should be fine not skipping a letter //
	result = itr.GetNextNumber(DECIMALSEPARATORTYPE_NONE);

	TargetPortNumber = Convert::WstringToInt(*result.get());
}

DLLEXPORT Leviathan::ConnectionInfo::ConnectionInfo(const sf::IpAddress &targetaddress, USHORT port) : HostName(nullptr), AddressGot(true), 
	TargetPortNumber(port), TargetHost(targetaddress)
{

}

DLLEXPORT Leviathan::ConnectionInfo::~ConnectionInfo(){

}
// ------------------ Packet extensions ------------------ //
sf::Packet& operator <<(sf::Packet& packet, const NetworkAckField &data){
	// First set the trivial data //
	packet << data.FirstPacketID << data.AckCount;
	// Now to fill in the ack data //
	for(char i = 0; i < data.AckCount/8; i++){
		packet << data.Acks[i];
	}

	return packet;
}

sf::Packet& operator >>(sf::Packet& packet, NetworkAckField &data){
	// Get data //
	packet >> data.FirstPacketID;
	packet >> data.AckCount;
	// Fill in the acks from the packet //
	data.Acks.resize(data.AckCount);
	for(char i = 0; i < data.AckCount/8; i++){
		packet >> data.Acks[i];
	}

	return packet;
}
// ------------------------------------ //
DLLEXPORT bool Leviathan::ConnectionInfo::Init(){
	// This might do something //
	if(!AddressGot){
		TargetHost = sf::IpAddress(Convert::WstringToString(*HostName.get()));
	}
	// Register us //
	NetworkHandler::Get()->_RegisterConnectionInfo(this);

	Logger::Get()->Info(L"ConnectionInfo: opening connection to host on "+Convert::StringToWstring(TargetHost.toString())+L" to port "
		+Convert::ToWstring(TargetPortNumber));

	return true;
}

DLLEXPORT void Leviathan::ConnectionInfo::Release(){
	// Remove us from the queue //
	NetworkHandler::Get()->_UnregisterConnectionInfo(this);
}
// ------------------------------------ //
DLLEXPORT shared_ptr<NetworkResponse> Leviathan::ConnectionInfo::SendRequestAndBlockUntilDone(shared_ptr<NetworkRequest> request, int maxtries /*= 2*/){
	// Send the request //
	shared_ptr<SentNetworkThing> sentrequest = SendPacketToConnection(request, maxtries);

	// Now we wait //
	sentrequest->WaitForMe->get_future().get();

	// Remove it //
	ObjectLock guard(*this);
	_PopMadeRequest(sentrequest, guard);

	return sentrequest->GotResponse;
}

DLLEXPORT shared_ptr<SentNetworkThing> Leviathan::ConnectionInfo::SendPacketToConnection(shared_ptr<NetworkRequest> request, int maxretries){
	ObjectLock guard(*this);
	// Generate a packet from the request //
	sf::Packet actualpackettosend;
	// We need a complete header with acks and stuff //
	_PreparePacketHeaderForPacket(++LastUsedID, actualpackettosend, true);

	// Generate packet object for the request //
	sf::Packet requestsdata = request->GeneratePacketForRequest();

	// Add the data to the actual packet //
	actualpackettosend.append(requestsdata.getData(), requestsdata.getDataSize());

	// We need to use the handlers socket //
	{
		ObjectLock guard(*NetworkHandler::Get());
		NetworkHandler::Get()->_Socket.send(actualpackettosend, TargetHost, TargetPortNumber);
	}

	// Add to the sent packets //
	shared_ptr<SentNetworkThing> tmprequestinfo(new SentNetworkThing(LastUsedID, request->GetExpectedResponseID(), request, shared_ptr<boost::promise<bool>>(new 
		boost::promise<bool>()), maxretries, request->GetTimeOutType(), request->GetTimeOutValue(), requestsdata, 1));

	WaitingRequests.push_back(tmprequestinfo);

	return tmprequestinfo;
}
// ------------------------------------ //
void Leviathan::ConnectionInfo::_PopMadeRequest(shared_ptr<SentNetworkThing> objectptr, ObjectLock &guard){
	VerifyLock(guard);

	for(auto iter = WaitingRequests.begin(); iter != WaitingRequests.end(); ++iter){

		if((*iter).get() == objectptr.get()){

			WaitingRequests.erase(iter);
			return;
		}
	}
}

void Leviathan::ConnectionInfo::_ResendRequest(shared_ptr<SentNetworkThing> toresend, ObjectLock &guard){
	VerifyLock(guard);
#ifdef _DEBUG
	Logger::Get()->Info(L"ConnectionInfo: resending packet");
#endif // _DEBUG
	// Generate a packet from the request //
	sf::Packet tosend;

	_PreparePacketHeaderForPacket(toresend->PacketNumber, tosend, toresend->IsArequest);

	// Add the packet data //
	tosend.append(toresend->AlmostCompleteData.getData(), toresend->AlmostCompleteData.getDataSize());

	// We need to use the handlers socket //
	{
		ObjectLock guard(*NetworkHandler::Get());
		NetworkHandler::Get()->_Socket.send(tosend, TargetHost, TargetPortNumber);
	}
}
// ------------------------------------ //
DLLEXPORT void Leviathan::ConnectionInfo::UpdateListening(){

	// Timeout stuff (if possible) //
	__int64 timems = Misc::GetTimeMs64();

	ObjectLock guard(*this);

	// Verify current ack number //
	// We only want to go back if there are ack objects that haven't made it to the receiver //
	bool FoundSent = false;
	for(int i = AcksNotConfirmedAsReceived.size()-1; i > -1; i--){
		// If we have successfully sent earlier ack packets we can set send count to 0 to resend them //
		if(FoundSent){
			if(AcksNotConfirmedAsReceived[i]->Received){
				AcksNotConfirmedAsReceived.erase(AcksNotConfirmedAsReceived.begin()+i);
				continue;
			}
			// This needs resending //
			AcksNotConfirmedAsReceived[i]->SendCount = 0;

		} else {
			// Check if we have sent this new packet //
			if(AcksNotConfirmedAsReceived[i]->Received){
				FoundSent = true;
				AcksNotConfirmedAsReceived.erase(AcksNotConfirmedAsReceived.begin()+i);
			}
		}
	}



	for(auto iter = WaitingRequests.begin(); iter != WaitingRequests.end(); ){
		// Check is it already here //
		auto itergot = SentPacketsConfirmedAsReceived.find((*iter)->PacketNumber);


		if(itergot != SentPacketsConfirmedAsReceived.end() && itergot->second){
			// It is here! //
			if(!(*iter)->IsArequest){
				// It is now done! //
				// We want to notify all waiters that it has been received //
				(*iter)->WaitForMe->set_value(true);
				iter = WaitingRequests.erase(iter);
				continue;
			}
		}


		// Check for resend //
		if((*iter)->TimeOutMS != -1){
			// Check based on different styles //
			switch((*iter)->PacketTimeoutStyle){
			case PACKAGE_TIMEOUT_STYLE_TIMEDMS:
				{
					if(timems-(*iter)->RequestStartTime > (*iter)->TimeOutMS){
movepacketsendattemptonexttry:

						// Move to next try //
						if(++(*iter)->AttempNumber > (*iter)->MaxTries && (*iter)->MaxTries > 0){

							Logger::Get()->Warning(L"ConnectionInfo: request reached maximum tries");
							// We want to notify all waiters that it failed //
							(*iter)->WaitForMe->set_value(false);
							iter = WaitingRequests.erase(iter);
							continue;
						}
						// Resend request //
						_ResendRequest((*iter), guard);
					}
				}
				break;
			case PACKAGE_TIMEOUT_STYLE_PACKAGESAFTERRECEIVED:
				{
					if((*iter)->PacketNumber+(*iter)->TimeOutMS < SentPacketsConfirmedAsReceived.rbegin()->first){

						goto movepacketsendattemptonexttry;
					}
				}
				break;
			}
		}

		// Move to next //
		++iter;
	}

	// Check which ack packets have been received //
	// Mark acks as successfully sent //
	for(size_t i = 0; i < AcksNotConfirmedAsReceived.size(); i++){

		auto iter = SentPacketsConfirmedAsReceived.find(AcksNotConfirmedAsReceived[i]->InsidePacket);


		if(iter != SentPacketsConfirmedAsReceived.end() && iter->second){

			// Mark as properly sent //
			AcksNotConfirmedAsReceived[i]->Received = true;

			// Erase from the main ack send map //
			AcksNotConfirmedAsReceived[i]->AcksInThePacket->RemoveMatchingPacketIDsFromMap(ReceivedPacketsNotifiedAsReceivedByUs);
		}
	}


	// TODO: send keep alive packet if it has been a while //

}

DLLEXPORT bool Leviathan::ConnectionInfo::IsThisYours(sf::Packet &packet, sf::IpAddress &sender, USHORT &sentport){
	// Check for matching sender with our target //
	if(sentport != TargetPortNumber || sender != TargetHost){
		// Not mine //
		return false;
	}
	// It is mine //
	Logger::Get()->Info(L"It is mine, in fact");

	// Handle incoming packet //
	int packetnumber = 0;

	if(!(packet >> packetnumber)){

		Logger::Get()->Error(L"Received package has invalid format");
	}

	NetworkAckField otherreceivedpackages;

	if(!(packet >> otherreceivedpackages)){

		Logger::Get()->Error(L"Received package has invalid format");
	}

	bool isrequest = false;

	if(!(packet >> isrequest)){

		Logger::Get()->Error(L"Received package has invalid format");
	}
	// Rest of the data is now the actual thing //

	// Handle the packet (hopefully the handling function queues it or something to not stall execution) //
	if(isrequest){
		// Generate a request object and make the interface handle it //


	} else {
		// Generate a response and pass to the interface //


	}
	ObjectLock guard(*this);

	// Handle resends based on ack field //
	otherreceivedpackages.SetPacketsReceivedIfNotSet(SentPacketsConfirmedAsReceived);

	// Report the packet as received //
	_VerifyAckPacketsAsSuccesfullyReceivedFromHost(packetnumber);

	return true;
}
// ------------------------------------ //
void Leviathan::ConnectionInfo::_VerifyAckPacketsAsSuccesfullyReceivedFromHost(int packetreceived){
	// Mark it as received //
	ObjectLock guard(*this);

	// Only set if we haven't already set it //
	auto iter = ReceivedPacketsNotifiedAsReceivedByUs.find(packetreceived);

	if(iter == ReceivedPacketsNotifiedAsReceivedByUs.end()){
		// We set this to false to keep track if we have told the sender that we got this packet (then it is true and can be removed from the map) //
		ReceivedPacketsNotifiedAsReceivedByUs[packetreceived] = false;
	}
}

void Leviathan::ConnectionInfo::_PreparePacketHeaderForPacket(int packetid, sf::Packet &tofill, bool isrequest){
	// First thing is the packet number //
	tofill << packetid;

	// Now the hard part, creating the ack table //

	// Actually this can be skipped if we can send a ack packet again //
	ObjectLock guard(*this);

	bool newrequired = true;

	for(size_t i = 0; i < AcksNotConfirmedAsReceived.size(); i++){
		if(AcksNotConfirmedAsReceived[i]->SendCount < MaxAckReduntancy){

			AcksNotConfirmedAsReceived[i]->SendCount++;

			tofill << *AcksNotConfirmedAsReceived[i]->AcksInThePacket;

			newrequired = false;
			break;
		}

	}

	if(newrequired){
		// First we need to determine which received packet to use as first value //

		if(ReceivedPacketsNotifiedAsReceivedByUs.size() > 0){

			bool foundstartval = ReceivedPacketsNotifiedAsReceivedByUs.find(LastSentConfirmID) != ReceivedPacketsNotifiedAsReceivedByUs.end();
			int lastval = ReceivedPacketsNotifiedAsReceivedByUs.rbegin()->first;

			if(LastSentConfirmID != -1 && foundstartval && (LastSentConfirmID+DEFAULT_ACKCOUNT <= lastval || 
				ReceivedPacketsNotifiedAsReceivedByUs.size() < (size_t)(1.5f*DEFAULT_ACKCOUNT)) && (LastSentConfirmID+4 <= lastval || 
				ReceivedPacketsNotifiedAsReceivedByUs.size() < (size_t)(6)))
			{
				// Current last sent is ok //

			} else {
				// Go back to beginning //
				LastSentConfirmID = ReceivedPacketsNotifiedAsReceivedByUs.begin()->first;
			}
		}

		// Create the ack field //
		AcksNotConfirmedAsReceived.push_back(shared_ptr<SentAcks>(new SentAcks(packetid, new NetworkAckField(LastSentConfirmID, 
			DEFAULT_ACKCOUNT, ReceivedPacketsNotifiedAsReceivedByUs))));

		// Put into the packet //
		tofill << *AcksNotConfirmedAsReceived.back()->AcksInThePacket;

		// We increment the last send, just for fun and to see what happens //
		LastSentConfirmID++;
	}

	// Mark as request, if it is one //
	tofill << isrequest;
}
// ------------------ SentNetworkThing ------------------ //
Leviathan::SentNetworkThing::SentNetworkThing(int packetid, int expectedresponseid, shared_ptr<NetworkRequest> request, shared_ptr<boost::promise<bool>> waitobject, 
	int maxtries, PACKET_TIMEOUT_STYLE howtotimeout, int timeoutvalue, const sf::Packet &packetsdata, int attempnumber /*= 1*/) : PacketNumber(packetid),
	ExpectedResponseID(expectedresponseid), OriginalRequest(request), WaitForMe(waitobject), MaxTries(maxtries), PacketTimeoutStyle(howtotimeout),
	TimeOutMS(timeoutvalue), AlmostCompleteData(packetsdata), AttempNumber(attempnumber)
{

}

Leviathan::SentNetworkThing::SentNetworkThing(int packetid, shared_ptr<NetworkResponse> response, shared_ptr<boost::promise<bool>> waitobject, 
	int maxtries, PACKET_TIMEOUT_STYLE howtotimeout, int timeoutvalue, const sf::Packet &packetsdata, int attempnumber /*= 1*/) : PacketNumber(packetid),
	ExpectedResponseID(-1), SentResponse(response), WaitForMe(waitobject), MaxTries(maxtries), PacketTimeoutStyle(howtotimeout),
	TimeOutMS(timeoutvalue), AlmostCompleteData(packetsdata), AttempNumber(attempnumber)
{

}
// ------------------ NetworkAckField ------------------ //
Leviathan::NetworkAckField::NetworkAckField(sf::Int32 firstpacketid, char maxacks, ReceivedPacketField &copyfrom) : FirstPacketID(firstpacketid), AckCount(0){

	// Id is -1 nothing should be copied //
	if(FirstPacketID == -1)
		return;

	auto iter = copyfrom.begin();

	for(; iter != copyfrom.end(); ++iter){
		if(iter->first >= firstpacketid){
			break;
		}
	}


	// Reserve space for data //
	Acks.reserve(maxacks);

	// Start filling it //
	for(char i = 0; i < maxacks; i++){

		int curcheckacknumber = firstpacketid+i;

		size_t vecelement = i/7;

		while(Acks.size() <= vecelement){

			Acks.push_back(0);
		}

		// If reached the end just put zeroes //
		if(iter == copyfrom.end()){
			AckCount++;
			continue;
		}

		if(iter->first == curcheckacknumber){
			// Set it //
setcurrentnumbertoackfield:

			Acks[vecelement] |= (1 << (i-vecelement));

			AckCount++;
			++iter;

		} else if(iter->first > curcheckacknumber){
			// We are missing these packets so let them be zeroed out //
			AckCount++;


		} else {
			// Advance iterator to reach this //
			for(; iter != copyfrom.end(); ++iter){
				if(iter->first >= curcheckacknumber){
					goto setcurrentnumbertoackfield;
				}
			}
		}
	}


	// We need to reset FirstPacketID if no acks set //
	if(AckCount == 0)
		FirstPacketID = -1;
}

DLLEXPORT void Leviathan::NetworkAckField::SetPacketsReceivedIfNotSet(ReceivedPacketField &copydatato){
	// We need to loop through all our acks and set them in the map //
	for(size_t i = 0; i < Acks.size(); i++){
		// Loop the individual bits //
		for(int bit = 0; bit < 8; bit++){

			// Check is it set //
			if(Acks[i] & (1 << bit)){
				// Set as received //
				copydatato[FirstPacketID+i*8+bit] = true;
			}
		}
	}
}

DLLEXPORT void Leviathan::NetworkAckField::RemoveMatchingPacketIDsFromMap(ReceivedPacketField &removefrom){
	// We need to loop through all our acks and erase them from the map (if set) //
	for(size_t i = 0; i < Acks.size(); i++){
		// Loop the individual bits //
		for(int bit = 0; bit < 8; bit++){

			// Check is it set //
			if(Acks[i] & (1 << bit)){
				// Remove it //
				removefrom.erase(FirstPacketID+i*8+bit);
			}
		}
	}
}
// ------------------ SentAcks ------------------ //
Leviathan::SentAcks::SentAcks(int packet, NetworkAckField* newddata) : InsidePacket(packet), AcksInThePacket(newddata), SendCount(1), Received(false){

}

Leviathan::SentAcks::~SentAcks(){
	SAFE_DELETE(AcksInThePacket);
}
