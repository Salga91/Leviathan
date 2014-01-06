#ifndef LEVIATHAN_NETWORKINTERFACE
#define LEVIATHAN_NETWORKINTERFACE
// ------------------------------------ //
#ifndef LEVIATHAN_DEFINE
#include "Define.h"
#endif
// ------------------------------------ //
// ---- includes ---- //


namespace Leviathan{

	class NetworkInterface{
	public:
		DLLEXPORT NetworkInterface();
		DLLEXPORT virtual ~NetworkInterface();


		// Handling functions //

		//! \brief Called by ConnectionInfo to handle incoming packets
		//!
		//! This function is responsible for interpreting the packet data and generating a response. If the response could take a long time to
		//! generate it is recommended to queue a task to the ThreadingManager. The connection parameter shouldn't be stored since it can become
		//! invalid after this function returns
		DLLEXPORT virtual void HandleRequestPacket(shared_ptr<NetworkRequest> request, ConnectionInfo* connection) THROWS;

		//! \brief Called by ConnectionInfo to verify that response is good.
		//!
		//! By default this will always return true, but can be overloaded to return false if response is no good.
		//! When returning false the connection will pretend that the response never arrived and possibly resends the request.
		DLLEXPORT virtual bool PreHandleResponse(shared_ptr<NetworkResponse> response, shared_ptr<NetworkRequest> originalrequest, 
			ConnectionInfo* connection);


		//! \brief Called by ConnectionInfo when it receives a response without a matching request object
		//!
		//! This is called when the host on the connection sends a response without a matching request. Usually the other program instance
		//! wants us to do something without expecting a response, for example they could want us to add a new message to our inbox without
		//! expecting a response (other than an ack which is automatically sent) from us. This function shouldn't throw any exceptions.
		DLLEXPORT virtual void HandleResponseOnlyPacket(shared_ptr<NetworkResponse> message, ConnectionInfo* connection) = 0;

	protected:

		//! \brief Utility function for subclasses to call for default handling
		//!
		//! Handles default types of request packages and returns true if processed.
		DLLEXPORT bool _HandleDefaultRequest(shared_ptr<NetworkRequest> request, ConnectionInfo* connectiontosendresult);
		//! \brief Utility function for subclasses to call for default handling of non-request responses
		//!
		//! Handles default types of response packages and returns true if processed.
		DLLEXPORT bool _HandleDefaultResponseOnly(shared_ptr<NetworkResponse> message, ConnectionInfo* connection);


	};

}
#endif