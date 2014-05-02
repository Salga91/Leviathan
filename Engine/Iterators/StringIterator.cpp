#include "Include.h"
// ------------------------------------ //
#ifndef LEVIATHAN_STRINGITERATOR
#include "StringIterator.h"
#endif
#include "utf8/checked.h"
using namespace Leviathan;
// ------------------------------------ //
DLLEXPORT Leviathan::StringIterator::StringIterator(StringDataIterator* iterator, bool TakesOwnership) : CurrentFlags(0), 
	HandlesDelete(TakesOwnership), DataIterator(iterator), CurrentStored(false), CurrentCharacter(-1)
{
#ifdef _DEBUG
	DebugMode = false;
#endif // _DEBUG
}

DLLEXPORT Leviathan::StringIterator::StringIterator(const string &text) : CurrentFlags(0), HandlesDelete(true), 
	DataIterator(new StringClassDataIterator<string>(text)), CurrentStored(false), CurrentCharacter(-1)
{
#ifdef _DEBUG
	DebugMode = false;
#endif // _DEBUG
}

DLLEXPORT Leviathan::StringIterator::StringIterator(const wstring &text) : CurrentFlags(0), HandlesDelete(true), 
	DataIterator(new StringClassDataIterator<wstring>(text)), CurrentStored(false), CurrentCharacter(-1)
{
#ifdef _DEBUG
	DebugMode = false;
#endif // _DEBUG
}

DLLEXPORT Leviathan::StringIterator::StringIterator(wstring* text) : CurrentFlags(0), HandlesDelete(true), 
	DataIterator(new StringClassPointerIterator<wstring>(text)), CurrentStored(false), CurrentCharacter(-1){
#ifdef _DEBUG
	DebugMode = false;
#endif // _DEBUG
}

DLLEXPORT Leviathan::StringIterator::StringIterator(string* text) : CurrentFlags(0), HandlesDelete(true), 
	DataIterator(new StringClassPointerIterator<string>(text)), CurrentStored(false), CurrentCharacter(-1){
#ifdef _DEBUG
	DebugMode = false;
#endif // _DEBUG
}

DLLEXPORT Leviathan::StringIterator::~StringIterator(){
	if(HandlesDelete){

		SAFE_DELETE(DataIterator);
	}
}
// ------------------------------------ //

DLLEXPORT void Leviathan::StringIterator::ReInit(StringDataIterator* iterator, bool TakesOwnership /*= false*/){
	// Remove the last iterator //
	if(HandlesDelete){

		SAFE_DELETE(DataIterator);
	}

	HandlesDelete = TakesOwnership;
	DataIterator = iterator;

	// Reset everything //
	CurrentCharacter = -1;
	CurrentStored = false;


	// Clear the flags //
	CurrentFlags = 0;
}

DLLEXPORT void Leviathan::StringIterator::ReInit(const wstring &text){
	ReInit(new StringClassDataIterator<wstring>(text), true);
}

DLLEXPORT void Leviathan::StringIterator::ReInit(const string &text){
	ReInit(new StringClassDataIterator<string>(text), true);
}

DLLEXPORT void Leviathan::StringIterator::ReInit(wstring* text){
	ReInit(new StringClassPointerIterator<wstring>(text), true);
}

DLLEXPORT void Leviathan::StringIterator::ReInit(string* text){
	ReInit(new StringClassPointerIterator<string>(text), true);
}

// ------------------------------------ //
void Leviathan::StringIterator::StartIterating(boost::function<ITERATORCALLBACK_RETURNTYPE()> functorun){

	// We want to skip multiple checks on same character so we skip checks on first character when starting except the beginning of the string //
	bool IsStartUpLoop = GetPosition() > 0 ? true: false;

	bool firstiter = true;
	if(IsStartUpLoop)
		firstiter = false;

	for(; DataIterator->IsPositionValid(); DataIterator->MoveToNextCharacter()){
#ifdef _DEBUG
		if(DebugMode){
			// Convert to UTF8 //

			wstring datathing = L"Iterator: iterating: "+Convert::ToWstring(GetPosition())+L" (";

			// Encode the character //
			utf8::append(GetCharacter(), back_inserter(datathing));

			datathing += L")";

			Logger::Get()->Write(datathing);
		}
#endif // _DEBUG

		// First iteration call is the same as the last so skip it //
		if(!firstiter){
#ifdef _DEBUG
			if(DebugMode){
				Logger::Get()->Write(L"Iterator: handle: CheckActiveFlags, HandleSpecialCharacters");
			}
#endif // _DEBUG

			if(CheckActiveFlags() == ITERATORCALLBACK_RETURNTYPE_STOP)
				break;

			// check current character //
			if(HandleSpecialCharacters() == ITERATORCALLBACK_RETURNTYPE_STOP)
				break;
		}

		firstiter = false;

#ifdef _DEBUG
		if(DebugMode){
			Logger::Get()->Write(L"Iterator: handle: call check function");
		}
#endif // _DEBUG

		// valid character/valid iteration call callback //
		ITERATORCALLBACK_RETURNTYPE retval = functorun();
		if(retval == ITERATORCALLBACK_RETURNTYPE_STOP)
			break;

		// Character changes after this //
		CurrentStored = false;
	}
}
// ------------------------------------ //
Leviathan::ITERATORCALLBACK_RETURNTYPE Leviathan::StringIterator::HandleSpecialCharacters(){
	// check should this special character be ignored //
	if(CurrentFlags & ITERATORFLAG_SET_IGNORE_SPECIAL)
		return ITERATORCALLBACK_RETURNTYPE_CONTINUE;
	
	// check for special characters //
	int character = GetCharacter();

	switch(character){
	case '\\':
		{
			// ignore next special character //
			CurrentFlags |= ITERATORFLAG_SET_IGNORE_SPECIAL;

#ifdef _DEBUG
			if(DebugMode){
				Logger::Get()->Write(L"Iterator: setting: WSTRINGITERATOR_IGNORE_SPECIAL");
			}
#endif // _DEBUG
		}
	break;
	case '"':
		{
			// a string //
			if(!(CurrentFlags & ITERATORFLAG_SET_INSIDE_STRING_DOUBLE)){
#ifdef _DEBUG
				if(DebugMode){
					Logger::Get()->Write(L"Iterator: setting: WSTRINGITERATOR_INSIDE_STRING_DOUBLE");
				}
#endif // _DEBUG
				// set //
				CurrentFlags |= ITERATORFLAG_SET_INSIDE_STRING_DOUBLE;

				// set as inside string //
				CurrentFlags |= ITERATORFLAG_SET_INSIDE_STRING;

			} else {
#ifdef _DEBUG
				if(DebugMode){
					Logger::Get()->Write(L"Iterator: set flag end: WSTRINGITERATOR_INSIDE_STRING_DOUBLE");
				}
#endif // _DEBUG
				// set ending flag //
				CurrentFlags |= ITERATORFLAG_SET_INSIDE_STRING_DOUBLE_END;
			}
		}
	break;
	case '\'':
		{
			// a string //
			if(!(CurrentFlags & ITERATORFLAG_SET_INSIDE_STRING_SINGLE)){
#ifdef _DEBUG
				if(DebugMode){
					Logger::Get()->Write(L"Iterator: setting: WSTRINGITERATOR_INSIDE_STRING_SINGLE");
				}
#endif // _DEBUG
				// set //
				CurrentFlags |= ITERATORFLAG_SET_INSIDE_STRING_SINGLE;

				// set as inside string //
				CurrentFlags |= ITERATORFLAG_SET_INSIDE_STRING;

			} else {
#ifdef _DEBUG
				if(DebugMode){
					Logger::Get()->Write(L"Iterator: set flag end: WSTRINGITERATOR_INSIDE_STRING_SINGLE");
				}
#endif // _DEBUG

				CurrentFlags |= ITERATORFLAG_SET_INSIDE_STRING_SINGLE_END;
			}
		}
		break;

	}

	return ITERATORCALLBACK_RETURNTYPE_CONTINUE;
}

Leviathan::ITERATORCALLBACK_RETURNTYPE Leviathan::StringIterator::CheckActiveFlags(){
	if(CurrentFlags & ITERATORFLAG_SET_STOP)
		return ITERATORCALLBACK_RETURNTYPE_STOP;

	// Reset 1 character long flags //
	if(CurrentFlags & ITERATORFLAG_SET_IGNORE_SPECIAL){
#ifdef _DEBUG
		if(DebugMode){
			Logger::Get()->Write(L"Iterator: flag: WSTRINGITERATOR_IGNORE_SPECIAL");
		}
#endif // _DEBUG

		// check should end now //
		if(CurrentFlags & ITERATORFLAG_SET_IGNORE_SPECIAL_END){
#ifdef _DEBUG
			if(DebugMode){
				Logger::Get()->Write(L"Iterator: flag ended: WSTRINGITERATOR_IGNORE_SPECIAL");
			}
#endif // _DEBUG
			// unset both //
			CurrentFlags &= ~ITERATORFLAG_SET_IGNORE_SPECIAL_END;
			CurrentFlags &= ~ITERATORFLAG_SET_IGNORE_SPECIAL;

		} else {
#ifdef _DEBUG
			if(DebugMode){
				Logger::Get()->Write(L"Iterator: flag ends next character: WSTRINGITERATOR_IGNORE_SPECIAL");
			}
#endif // _DEBUG
			// set to end next character //
			CurrentFlags |= ITERATORFLAG_SET_IGNORE_SPECIAL_END;
		}
	}

	// reset end flags before we process this cycle further //
	if(CurrentFlags & ITERATORFLAG_SET_INSIDE_STRING_DOUBLE_END){
#ifdef _DEBUG
		if(DebugMode){
			Logger::Get()->Write(L"Iterator: flag ends: WSTRINGITERATOR_INSIDE_STRING_DOUBLE");
		}
#endif // _DEBUG
		// unset flag //
		CurrentFlags &= ~ITERATORFLAG_SET_INSIDE_STRING_DOUBLE;
		CurrentFlags &= ~ITERATORFLAG_SET_INSIDE_STRING_DOUBLE_END;
	}

	if(CurrentFlags & ITERATORFLAG_SET_INSIDE_STRING_SINGLE_END){
#ifdef _DEBUG
		if(DebugMode){
			Logger::Get()->Write(L"Iterator: flag ends: WSTRINGITERATOR_INSIDE_STRING_SINGLE");
		}
#endif // _DEBUG
		// unset flag //
		CurrentFlags &= ~ITERATORFLAG_SET_INSIDE_STRING_SINGLE;
		CurrentFlags &= ~ITERATORFLAG_SET_INSIDE_STRING_SINGLE_END;
	}

	// Check can we unset the whole string flag //
	if(CurrentFlags & ITERATORFLAG_SET_INSIDE_STRING){
		if(!(CurrentFlags & ITERATORFLAG_SET_INSIDE_STRING_DOUBLE) && !(CurrentFlags & ITERATORFLAG_SET_INSIDE_STRING_SINGLE)){
	#ifdef _DEBUG
			if(DebugMode){
				Logger::Get()->Write(L"Iterator: flag ends: WSTRINGITERATOR_INSIDE_STRING");
			}
	#endif // _DEBUG
			// can unset this //
			CurrentFlags &= ~ITERATORFLAG_SET_INSIDE_STRING;
		}
	}


	return ITERATORCALLBACK_RETURNTYPE_CONTINUE;
}
// ------------------------------------ //
#ifdef _DEBUG
DLLEXPORT void Leviathan::StringIterator::SetDebugMode(const bool &mode){
	DebugMode = true;
}
#endif

DLLEXPORT size_t Leviathan::StringIterator::GetLastValidCharIndex() const{
	return DataIterator->GetLastValidIteratorPosition();
}
// ------------------------------------ //
DLLEXPORT int Leviathan::StringIterator::GetCharacter(size_t forward /*= 0*/){
	// Special case for current character //
	if(!forward){

		if(!CurrentStored){

			DataIterator->GetNextCharCode(CurrentCharacter, 0);
			CurrentStored = true;
		}

		return CurrentCharacter;
	}

	// Get the character from our iterator and store it to a temporary value and then return it //
	int tmpval;
	DataIterator->GetNextCharCode(tmpval, forward);

	return tmpval;
}
// ------------------------------------ //
DLLEXPORT bool Leviathan::StringIterator::MoveToNext(){
	DataIterator->MoveToNextCharacter();
	return DataIterator->IsPositionValid();
}

DLLEXPORT size_t Leviathan::StringIterator::GetPosition(){
	return DataIterator->CurrentIteratorPosition();
}
// ------------------ Iterating functions ------------------ //
Leviathan::ITERATORCALLBACK_RETURNTYPE Leviathan::StringIterator::FindFirstQuotedString(IteratorPositionData* data, QUOTETYPE quotes){

	int currentcharacter = GetCharacter();

	bool TakeChar = true;
	bool End = false;


	switch(quotes){
	case QUOTETYPE_BOTH:
		{
			if(CurrentFlags & ITERATORFLAG_SET_INSIDE_STRING){
				// check if we are on the quotes, because we don't want those //
				if(currentcharacter == '"' || currentcharacter == '\''){
					// if we aren't ignoring special disallow //
					if(!(CurrentFlags & ITERATORFLAG_SET_IGNORE_SPECIAL))
						TakeChar = false;
				}

			} else {
				End = true;
			}
		}
		break;
	case QUOTETYPE_SINGLEQUOTES:
		{
			if(CurrentFlags & ITERATORFLAG_SET_INSIDE_STRING_SINGLE){
				// check if we are on the quotes, because we don't want those //
				if(currentcharacter == '\''){
					// if we aren't ignoring special disallow //
					if(!(CurrentFlags & ITERATORFLAG_SET_IGNORE_SPECIAL))
						TakeChar = false;
				}

			} else {
				End = true;
			}
		}
		break;
	case QUOTETYPE_DOUBLEQUOTES:
		{
			if(CurrentFlags & ITERATORFLAG_SET_INSIDE_STRING_DOUBLE){
				// check if we are on the quotes, because we don't want those //
				if(currentcharacter == '"'){
					// if we aren't ignoring special disallow //
					if(!(CurrentFlags & ITERATORFLAG_SET_IGNORE_SPECIAL))
						TakeChar = false;
				}

			} else {
				End = true;
			}
		}
		break;
	}

	if(End){
		// if we have found at least a character we can end this here //
		if(data->Positions.X != -1){
			// Set the last character to two before this (skip the current and " and end there) //
			data->Positions.Y = GetPosition()-2;
			return ITERATORCALLBACK_RETURNTYPE_STOP;
		}
	} else if(TakeChar){
		// check is this first quoted character //
		if(data->Positions.X == -1){
			// first position! //
			data->Positions.X = GetPosition();

		}
	}

	return ITERATORCALLBACK_RETURNTYPE_CONTINUE;
}

Leviathan::ITERATORCALLBACK_RETURNTYPE Leviathan::StringIterator::FindNextNormalCharacterString(IteratorPositionData* data, int stopflags){

	int currentcharacter = GetCharacter();

	bool IsValid = true;


	if((stopflags & UNNORMALCHARACTER_TYPE_LOWCODES || stopflags & UNNORMALCHARACTER_TYPE_WHITESPACE)
		&& !(CurrentFlags & ITERATORFLAG_SET_INSIDE_STRING))
	{
		if(currentcharacter <= ' '){
			IsValid = false;
			goto invalidcodelabelunnormalcharacter;
		}
	}

	if(stopflags & UNNORMALCHARACTER_TYPE_NON_ASCII){

		if(CurrentFlags & ITERATORFLAG_SET_INSIDE_STRING || !(
			(currentcharacter >= '0' && currentcharacter <= '9') || (currentcharacter >= 'A' && currentcharacter <= 'Z') || 
			(currentcharacter >= 'a' && currentcharacter <= 'z')))
		{
			IsValid = false;
			goto invalidcodelabelunnormalcharacter;
		}
	}

	if(stopflags & UNNORMALCHARACTER_TYPE_CONTROLCHARACTERS && !(CurrentFlags & ITERATORFLAG_SET_INSIDE_STRING)
		&& !(CurrentFlags & ITERATORFLAG_SET_IGNORE_SPECIAL))
	{
		if(((currentcharacter <= '/' && currentcharacter >= '!') || (currentcharacter <= '@' && currentcharacter >= ':') 
			|| (currentcharacter <= '`' && currentcharacter >= '[')	|| (currentcharacter <= '~' && currentcharacter >= '{')) 
			&& !(currentcharacter == '_' || currentcharacter == '-'))
		{
			IsValid = false;
			goto invalidcodelabelunnormalcharacter;
		}
	}

	if(IsValid){
		// check is this first character //
		if(data->Positions.X == -1){
			// first position! //
			data->Positions.X = GetPosition();
		}

	} else {

invalidcodelabelunnormalcharacter:


		// check for end //
		if(data->Positions.X != -1){
			// ended //
			data->Positions.Y = GetPosition()-1;
			return ITERATORCALLBACK_RETURNTYPE_STOP;
		}
	}

	return ITERATORCALLBACK_RETURNTYPE_CONTINUE;
}

Leviathan::ITERATORCALLBACK_RETURNTYPE Leviathan::StringIterator::FindNextNumber(IteratorNumberFindData* data, DECIMALSEPARATORTYPE decimal){

	// Check is the current element a part of a number //

	int currentcharacter = GetCharacter();

	bool IsValid = false;


	if((currentcharacter >= 48) && (currentcharacter <= 57)){
		// Is a plain old digit //
		IsValid = true;

	} else {
		// Check is it a decimal separator (1 allowed) or a negativity sign in front //
		if(currentcharacter == '+' || currentcharacter == '-'){

			if((data->DigitsFound < 1) && (!data->NegativeFound)){
				IsValid = true;
			}
			data->NegativeFound = true;
		} else if (((currentcharacter == '.') && ((decimal == DECIMALSEPARATORTYPE_DOT) || (decimal == DECIMALSEPARATORTYPE_BOTH))) ||
			((currentcharacter == ',') && ((decimal == DECIMALSEPARATORTYPE_COMMA) || (decimal == DECIMALSEPARATORTYPE_BOTH))))
		{
			if((!data->DecimalFound) && (data->DigitsFound > 0)){
				IsValid = true;
				data->DecimalFound = true;
			}
		}
	}

	if(IsValid){
		// check is this first digit //
		data->DigitsFound++;
		if(data->Positions.X == -1){
			// first position! //

			data->Positions.X = GetPosition();
		}

	} else {
		// check for end //
		if(data->Positions.X != -1){
			// ended //
			data->Positions.Y = GetPosition()-1;
			return ITERATORCALLBACK_RETURNTYPE_STOP;
		}

	}
	return ITERATORCALLBACK_RETURNTYPE_CONTINUE;
}

Leviathan::ITERATORCALLBACK_RETURNTYPE Leviathan::StringIterator::FindUntilEquality(IteratorAssignmentData* data, EQUALITYCHARACTER equality){
	// check is current element a valid element //
	int charvalue = GetCharacter();

	bool IsValid = true;
	bool IsStop = false;


	// Skip if this is a space //
	if(charvalue < 33){
		// Not allowed in a name //
		IsValid = false;
	}

	if(equality == EQUALITYCHARACTER_TYPE_ALL){
		// check for all possible value separators //
		if(charvalue == '=' || charvalue == ':'){

			if(!(CurrentFlags & ITERATORFLAG_SET_IGNORE_SPECIAL)){
				// If ignored don't stop //
				IsStop = true;
			}
		}
	} else if(equality == EQUALITYCHARACTER_TYPE_EQUALITY){
		// Check for an equality sign //
		if(charvalue == '='){
			if(!(CurrentFlags & ITERATORFLAG_SET_IGNORE_SPECIAL)){
				IsStop = true;
			}
		}
	} else if (equality == EQUALITYCHARACTER_TYPE_DOUBLEDOTSTYLE){
		// Check does it match the characters //
		if(charvalue == ':'){
			if(!(CurrentFlags & ITERATORFLAG_SET_IGNORE_SPECIAL)){
				IsStop = true;
			}
		}
	}

	if(!IsStop){
		// end if end already found //
		if(data->SeparatorFound){

			return ITERATORCALLBACK_RETURNTYPE_STOP;
		}
	} else {
		data->SeparatorFound = true;
		IsValid = false;
	}

	if(IsValid){
		// Check is this the first character //
		if(data->Positions.X == -1){
			// first position! //
			data->Positions.X = GetPosition();
		} else {
			// Set end to this valid character //
			data->Positions.Y = GetPosition()-1;
		}
	}


	return ITERATORCALLBACK_RETURNTYPE_CONTINUE;
}


Leviathan::ITERATORCALLBACK_RETURNTYPE Leviathan::StringIterator::SkipSomething(IteratorCharacterData* data, int additionalskip){

	// We can just return if we are inside a string //
	if(CurrentFlags & ITERATORFLAG_SET_INSIDE_STRING){
		return ITERATORCALLBACK_RETURNTYPE_STOP;
	}

	int curchara = GetCharacter();

	// cCheck does the character match what is being skipped //

	if(additionalskip & UNNORMALCHARACTER_TYPE_LOWCODES){
		if(curchara <= 32)
			return ITERATORCALLBACK_RETURNTYPE_CONTINUE;
	}

	if(curchara == data->CharacterToUse){
		// We want to skip it //
		return ITERATORCALLBACK_RETURNTYPE_CONTINUE;
	}

	// didn't match to be skipped characters //
	return ITERATORCALLBACK_RETURNTYPE_STOP;
}

Leviathan::ITERATORCALLBACK_RETURNTYPE Leviathan::StringIterator::FindUntilSpecificCharacter(IteratorFindUntilData* data, int character){

	// Can this character be added //
	bool ValidChar = true;

	// We can just continue if we are inside a string //
	if(!(CurrentFlags & ITERATORFLAG_SET_INSIDE_STRING)){
		// Check did we encounter stop character //
		if(GetCharacter() == character){
			// Skip if ignoring special characters //
			if(!(CurrentFlags & ITERATORFLAG_SET_IGNORE_SPECIAL)){
				// Not valid character //
				ValidChar = false;
				// We must have started to encounter the stop character //
				if(data->Positions.X != -1){
					// We encountered the stop character //
					data->FoundEnd = true;
				}
			}
		}
	}

	if(ValidChar){
		// valid character set start if not already set //
		if(data->Positions.X == -1){
			data->Positions.X = GetPosition();
			data->Positions.Y = data->Positions.X;
		}
		return ITERATORCALLBACK_RETURNTYPE_CONTINUE;
	}
	// let's stop if we have found something //
	if(data->Positions.X != -1){
		// This should be fine to get here //
		data->Positions.Y = GetPosition();
		return ITERATORCALLBACK_RETURNTYPE_STOP;
	}

	// haven't found anything, we'll need to find something //
	return ITERATORCALLBACK_RETURNTYPE_CONTINUE;
}
