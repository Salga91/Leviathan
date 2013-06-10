#include "Include.h"
// ------------------------------------ //
#ifndef LEVIATHAN_TYPES
#include "Types.h"
#endif
using namespace Leviathan;
// ------------------------------------ //
// --- AllocatedBinaryBlock --- //
AllocatedBinaryBlock::AllocatedBinaryBlock(){

}
AllocatedBinaryBlock::AllocatedBinaryBlock(unsigned char* ptr, int elements, bool release){
	Buffer = ptr;
	BufferSize = elements;
	Release = release;
}
AllocatedBinaryBlock::~AllocatedBinaryBlock(){
	if(Release){
		delete Buffer;
	}
}
unsigned char* AllocatedBinaryBlock::Get(int& Elements){
	Elements = BufferSize;
	return Buffer;
}
	//private:
	//	char* Buffer;
	//	int BufferSize;

	//	bool Release;
	//};


// ---- IntWstring ---- //
IntWstring::IntWstring(){
	Wstr = NULL;
	Value = -1;
}
IntWstring::IntWstring(const wstring& wstr, int value){
	Wstr = new wstring(wstr);
	Value = value;
}
IntWstring::~IntWstring(){
	if(Wstr){
		delete Wstr;
		Wstr = NULL;
	}
}

wstring* IntWstring::GetString(){
	return Wstr;
}

int IntWstring::GetValue(){
	return Value;
}

void IntWstring::SetString(wstring& wstr){
	if(Wstr){
		delete Wstr;
		Wstr = NULL;
	}
	Wstr = new wstring(wstr);
}

void IntWstring::SetValue(int value){
	Value = value;
}

// ---- Int2 ---- //
Int2::Int2(){
	X = 0;
	Y = 0;
}
Int2::Int2(int x, int y){
	X = x;
	Y = y;
}
Int2::Int2(int data){
	X = data;
	Y = data;
}

DLLEXPORT Leviathan::Int2::Int2(const Int2 &other){
	X = other.X;
	Y = other.Y;
}

Int2 Int2::operator +(const Int2 &val){
	return Int2(X+val.X, Y+val.Y);
}
int Int2::operator[](const int nIndex) const{
	switch(nIndex){
	case 0: return X;
	case 1: return Y;
	default: __assume(0);
	}
}
// ---- Int3 ---- //
Int3::Int3(){
	X = 0;
	Y = 0;
	Z = 0;
}
Int3::Int3(int x, int y, int z){
	X = x;
	Y = y;
	Z = z;
}
Int3::Int3(int data){
	// save a bit of space //
	X = Y = Z = data;
}

Int3 Int3::operator +(const Int3 &val){
	return Int3(X+val.X, Y+val.Y, Z+val.Z);
}
int Int3::operator[](const int nIndex) const{
	switch(nIndex){
	case 0: return X;
	case 1: return Y;
	case 2: return Z;
	default: __assume(0);
	}
}

DLLEXPORT Int3 Leviathan::Int3::operator-(const Int3& other) const{
	return Int3(X-other.X, Y-other.Y, Z-other.Z);
}

DLLEXPORT int Leviathan::Int3::AddAllTogether() const{
	return X+Y+Z;
}

Leviathan::Int1::Int1(){
	iVal = VAL_NOUPDATE;
}

Leviathan::Int1::Int1(int data){
	iVal = data;
}

Leviathan::Int1 Leviathan::Int1::operator+(const Int1& val){
	return Int1(val.GetIntValue());
}

//int Leviathan::Int1::operator[]() const{
//	return iVal;
//}

Leviathan::Int1::operator int() const{
	return iVal;
}

int Leviathan::Int1::GetIntValue() const{
	return iVal;
}

void Leviathan::Int1::SetIntValue(int val){
	iVal = val;
}

// ----- CharWithIndex ------ //

Leviathan::CharWithIndex::CharWithIndex(){
	Char = L' ';
	Index = -1;
}

Leviathan::CharWithIndex::CharWithIndex(wchar_t character, int index) : Char(character), Index(index){
	
}

Leviathan::CharWithIndex::~CharWithIndex(){

}

// ------------ UINT4 ---------------- //
DLLEXPORT Leviathan::UINT4::UINT4(UINT u1, UINT u2, UINT u3, UINT u4){
	X = u1;
	Y = u2;
	Z = u3;
	W = u4;
}

DLLEXPORT Leviathan::UINT4::UINT4(){
}

DLLEXPORT Leviathan::UINT4::operator UINT*(){
	return &X;
}

//DLLEXPORT Leviathan::UINT4::operator UINT*(){
//	switch(nIndex){
//	case 0: return &X;
//	case 1: return &Y;
//	case 2: return &Z;
//	case 3: return &W;
//	default: __assume(0);
//	}
//}

//DLLEXPORT UINT& Leviathan::UINT4::operator[](const int nIndex){
//	switch(nIndex){
//	case 0: return X;
//	case 1: return Y;
//	case 2: return Z;
//	case 3: return W;
//	default: __assume(0);
//	}
//}