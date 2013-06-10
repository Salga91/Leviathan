#ifndef LEVIATHAN_TYPES
#define LEVIATHAN_TYPES
// ----------------- //
#ifndef LEVIATHAN_INCLUDE
#include "Include.h"
#endif
namespace Leviathan{
		

#define NORMALIZATION_TOLERANCE	1e-6f


	// a hacky combine class //
	template<class Base, class Base2>
	class CombinedClass : public Base, public Base2{
	public:
		DLLEXPORT void ThisIsCombinedClassType() const{
			return;
		}
		DLLEXPORT const wstring& GetBaseClassNames() const{

			return wstring(Convert::StringToWstring(string(typeid(Base).name()+" , "+typeid(Base2).name())));
		}
	};

	// just a key index class //
	class CharWithIndex{
	public:
		CharWithIndex();
		CharWithIndex(wchar_t character, int index);
		~CharWithIndex();

		wchar_t Char;
		int Index;
	};

	struct IntWstring{
	public:
		IntWstring();
		IntWstring(const wstring& wstr, int value);
		~IntWstring();

		wstring* GetString();
		int GetValue();

		void SetString(wstring& wstr);
		void SetValue(int value);

		wstring* Wstr;
		int Value;
	};

	class AllocatedBinaryBlock{
	public:
		AllocatedBinaryBlock();
		AllocatedBinaryBlock(unsigned char* ptr, int elements, bool release);
		~AllocatedBinaryBlock();
		//~AllocatedBinaryBlock(bool force);

		unsigned char* Get(int& Elements);

	private:
		unsigned char* Buffer;
		int BufferSize; // this is actually size of the array NOT size of array in BYTES

		bool Release;
	};

	struct Int1{
	public:
		DLLEXPORT Int1();
		DLLEXPORT Int1(int data);

		// ------------------------------------ //
		DLLEXPORT Int1 operator +(const Int1& val);
		//DLLEXPORT int operator[]() const;

		DLLEXPORT operator int() const;

		DLLEXPORT int GetIntValue() const;
		DLLEXPORT void SetIntValue(int val);
		// ------------------------------------ //

		int iVal;
	};

	struct Int2{
	public:
		DLLEXPORT Int2();
		DLLEXPORT Int2(int x, int y);
		DLLEXPORT explicit Int2(int data);
		DLLEXPORT Int2(const Int2 &other);

		// ------------------------------------ //
		DLLEXPORT Int2 operator +(const Int2 &val);
		DLLEXPORT int operator[](const int nIndex) const;
		// ------------------------------------ //

		DLLEXPORT void SetData(const int &data){ X = data; Y = data; };
		DLLEXPORT void SetData(const int &data1, const int &data2){ X = data1; Y = data2; };

		int X, Y;
	};

	struct Int3{
	public:
		DLLEXPORT Int3();
		DLLEXPORT Int3(int x, int y, int z);
		DLLEXPORT explicit Int3(int data);

		// ------------------------------------ //
		DLLEXPORT Int3 operator +(const Int3 &val);
		DLLEXPORT int operator[](const int nIndex) const;
		DLLEXPORT Int3 operator -(const Int3& other) const;
		DLLEXPORT int AddAllTogether() const;
		// ------------------------------------ //

		int X, Y, Z;
	};

	struct UINT4{
	public:
		DLLEXPORT UINT4(UINT u1, UINT u2, UINT u3, UINT u4);
		DLLEXPORT UINT4();
		DLLEXPORT operator UINT*();
		//DLLEXPORT UINT& operator[](const int nIndex);
		//DLLEXPORT UINT4 operator*(UINT);

		UINT X, Y, Z, W;
	};

	//// not required //
	struct Float1{
	public:
		DLLEXPORT inline Float1(){
		}
		DLLEXPORT inline Float1(const float &data){
			X = data;
		}

		DLLEXPORT inline operator float& (){
			return X;
		}

		DLLEXPORT operator float() const{
			return X;
		}
		// ------------------------------------ //
		DLLEXPORT inline Float1 operator +(const Float1& val){
			return X+val.X;
		}
		DLLEXPORT inline Float1 operator +(const float& val){
			return X+val;
		}

		DLLEXPORT inline float GetFloatValue() const{
			return X;
		}
		DLLEXPORT inline void SetFloatValue(const float &val){
			X = val;
		}
		// ------------------------------------ //

		float X;
	};

	// ----------------- Float types ------------------- //
	// refactored to match declarations in ozz vec_float //

	// functions inlined just like in ozz, for speed, I guess //
	// mostly rewritten to match ozz implementation, just in case and not to break anything //

	struct Float2{
	public:
		DLLEXPORT inline Float2(){};
		DLLEXPORT inline Float2(float x, float y){
			X = x;
			Y = y;
		}
		DLLEXPORT inline explicit Float2(float both){
			X = Y = both;
		}

		// access operator //
		DLLEXPORT inline float& operator[](const int &nindex){
			switch (nindex){
			case 0: return X;
			case 1: return Y;
				// this should NEVER be hit //
			default: __assume(0);
			}
		}

		// ------------------- Operators ----------------- //
		// add elements //
		DLLEXPORT inline Float2 operator +(const Float2 &val) const{
			return Float2(X+val.X, Y+val.Y);
		}
		// subtracts all elements //
		DLLEXPORT inline Float2 operator-(const Float2 &val) const{
			return Float2(X-val.X, Y-val.Y);
		}
		// negates all elements //
		DLLEXPORT inline Float2 operator-() const{
			return Float2(-X, -Y);
		}
		// multiplies elements together //
		DLLEXPORT inline Float2 operator*(const Float2 &val) const{
			return Float2(X*val.X, Y*val.Y);
		}
		// multiply  by scalar f //
		DLLEXPORT inline Float2 operator*(float f) const{
			return Float2(X*f, Y*f);
		}
		// divides all elements //
		DLLEXPORT inline Float2 operator/(const Float2 &val) const{
			return Float2(X/val.X, Y/val.Y);
		}
		// divides by float //
		DLLEXPORT inline Float2 operator/(float f) const{
			return Float2(X/f, Y/f);
		}
		// ---- comparison operators ---- //
		// element by element comparison with operators //
		DLLEXPORT inline bool operator <(const Float2 &other) const{
			return X < other.X && Y < other.Y;
		};
		DLLEXPORT inline bool operator <=(const Float2 &other) const{
			return X <= other.X && Y <= other.Y;
		};
		DLLEXPORT inline bool operator >(const Float2 &other) const{
			return X > other.X && Y > other.Y;
		};
		DLLEXPORT inline bool operator >=(const Float2 &other) const{
			return X >= other.X && Y >= other.Y;
		};
		DLLEXPORT inline bool operator ==(const Float2 &other) const{
			return X == other.X && Y == other.Y;
		};
		DLLEXPORT inline bool operator !=(const Float2 &other) const{
			return X != other.X && Y != other.Y;
		};
		// ------------------ Functions ------------------ //
		DLLEXPORT inline float GetX() const{return X;}
		DLLEXPORT inline float GetY() const{return Y;}
		DLLEXPORT inline void SetX(const float &val){ X = val; };
		DLLEXPORT inline void SetY(const float &val){ Y= val; };

		// add all elements together //
		DLLEXPORT inline float HAdd() const{
			return X+Y;
		}
		// getting min and max of objects //
		DLLEXPORT inline Float2 MinElements(const Float2 &other) const{
			return Float2(X < other.X ? X : other.X, Y < other.Y ? Y : other.Y);
		}
		DLLEXPORT inline Float2 MaxElements(const Float2 &other) const{
			return Float2(X > other.X ? X : other.X, Y > other.Y ? Y : other.Y);
		}
		// value clamping //
		DLLEXPORT inline Float2 Clamp(const Float2 &min, const Float2 &max){
			const Float2 minval = this->MinElements(max);
			return min.MaxElements(minval);
		}

		// ----------------- Vector math ------------------- //
		// dot product of the vectors //
		DLLEXPORT inline float Dot(const Float2 &val) const{
			return X*val.X+Y*val.Y;
		}
		// length of the vector //
		DLLEXPORT inline float Length() const{
			return sqrt(X*X+Y*Y);
		}
		// normalizes the vector //
		DLLEXPORT inline Float2 Normalize() const{
			const float length = Length();
			return Float2(X/length, Y/length);
		}
		// safe version of normalization //
		DLLEXPORT inline Float2 NormalizeSafe(const Float2 &safer) const{
			// security //
			assert(safer.IsNormalized() && "safer not normalized");
			const float len = X*X+Y*Y;
			if(len == 0){
				return safer;
			}
			const float length = sqrt(len);
			return Float2(X/length, Y/length);
		}
		// checks is the vector normalized //
		DLLEXPORT inline bool IsNormalized() const{
			// is absolute -1.f under normalization tolerance //
			return fabs(X*X+Y*Y -1.0f) < NORMALIZATION_TOLERANCE;
		}
		// does linear interpolation between vectors and coefficient f, not limited to range [0,1], courtesy of ozz-animation //
		DLLEXPORT inline Float2 Lerp(const Float2 &other, float f) const{
			return Float2((other.X-X) * f + X, (other.Y-Y)*f+Y);
		}
		// compares distance between vectors to tolerance, returns true if less //
		DLLEXPORT inline bool Compare(const Float2 &other, float tolerance) const{
			const Float2 difference = (*this)-other;
			return difference.Dot(difference) < tolerance*tolerance;
		}
		// ------------------------------------ //
		// static returns //
		// creates a Float2 with all zeros //
		DLLEXPORT inline static Float2 zero(){
			return Float2(0.f, 0.f);
		}
		// creates a Float2 with all ones //
		DLLEXPORT inline static Float2 one(){
			return Float2(1.f, 1.f);
		}

		// unitary vector x, to work with ozz declarations //
		DLLEXPORT inline static Float2 x_asix(){
			return Float2(1.f, 0.f);
		}
		// unitary vector y //
		DLLEXPORT inline static Float2 y_axis(){
			return Float2(0.f, 1.f);
		}

		// data //
		float X, Y; 
	};
	struct Float3{
	public:
		DLLEXPORT Float3(){};
		DLLEXPORT Float3(float x, float y, float z){
			X = x;
			Y = y;
			Z = z;
		}
		DLLEXPORT Float3(Float2 floats, float z){
			X = floats.X;
			Y = floats.Y;
			Z = z;
		}
		DLLEXPORT explicit Float3(float data){
			X = Y = Z = data;
		}
		DLLEXPORT Float3(const Float3 &other){
			// copy values //
			X = other.X;
			Y = other.Y;
			Z = other.Z;
		}

		// access operator //
		DLLEXPORT inline float& operator[](const int &nindex){
			switch (nindex){
			case 0: return X;
			case 1: return Y;
			case 2: return Z;
				// this should NEVER be hit //
			default: __assume(0);
			}
		}

		// ------------------- Operators ----------------- //
		// add elements //
		DLLEXPORT inline Float3 operator +(const Float3 &val) const{
			return Float3(X+val.X, Y+val.Y, Z+val.Z);
		}
		// subtracts all elements //
		DLLEXPORT inline Float3 operator-(const Float3 &val) const{
			return Float3(X-val.X, Y-val.Y, Z-val.Z);
		}
		// negates all elements //
		DLLEXPORT inline Float3 operator-() const{
			return Float3(-X, -Y, -Z);
		}
		// multiplies elements together //
		DLLEXPORT inline Float3 operator*(const Float3 &val) const{
			return Float3(X*val.X, Y*val.Y, Z*val.Z);
		}
		// multiply  by scalar f //
		DLLEXPORT inline Float3 operator*(float f) const{
			return Float3(X*f, Y*f, Z*f);
		}
		// divides all elements //
		DLLEXPORT inline Float3 operator/(const Float3 &val) const{
			return Float3(X/val.X, Y/val.Y, Z/val.Z);
		}
		// divides by float //
		DLLEXPORT inline Float3 operator/(float f) const{
			return Float3(X/f, Y/f, Z/f);
		}
		// ---- comparison operators ---- //
		// element by element comparison with operators //
		DLLEXPORT inline bool operator <(const Float3 &other) const{
			return X < other.X && Y < other.Y && Z < other.Z;
		};
		DLLEXPORT inline bool operator <=(const Float3 &other) const{
			return X <= other.X && Y <= other.Y && Z <= other.Z;
		};
		DLLEXPORT inline bool operator >(const Float3 &other) const{
			return X > other.X && Y > other.Y && Z > other.Z;
		};
		DLLEXPORT inline bool operator >=(const Float3 &other) const{
			return X >= other.X && Y >= other.Y && Z > other.Z;
		};
		DLLEXPORT inline bool operator ==(const Float3 &other) const{
			return X == other.X && Y == other.Y && Z == other.Z;
		};
		DLLEXPORT inline bool operator !=(const Float3 &other) const{
			return X != other.X && Y != other.Y && Z != other.Z;
		};
		// ------------------ Functions ------------------ //
		DLLEXPORT inline float GetX() const{return X;};
		DLLEXPORT inline float GetY() const{return Y;};
		DLLEXPORT inline float GetZ() const{return Z;};
		DLLEXPORT inline void SetX(const float &val){ X = val; };
		DLLEXPORT inline void SetY(const float &val){ Y= val; };
		DLLEXPORT inline void SetZ(const float &val){ Z= val; };

		// add all elements together //
		DLLEXPORT inline float HAdd() const{
			return X+Y+Z;
		}
		// getting min and max of objects //
		DLLEXPORT inline Float3 MinElements(const Float3 &other) const{
			return Float3(X < other.X ? X : other.X, Y < other.Y ? Y : other.Y, Z < other.Z ? Z : other.Z);
		}
		DLLEXPORT inline Float3 MaxElements(const Float3 &other) const{
			return Float3(X > other.X ? X : other.X, Y > other.Y ? Y : other.Y, Z > other.Z ? Z : other.Z);
		}
		// value clamping //
		DLLEXPORT inline Float3 Clamp(const Float3 &min, const Float3 &max){
			const Float3 minval = this->MinElements(max);
			return min.MaxElements(minval);
		}

		// ----------------- Vector math ------------------- //
		// dot product of the vectors //
		DLLEXPORT inline float Dot(const Float3 &val) const{
			return X*val.X+Y*val.Y+Z*val.Z;
		}
		DLLEXPORT inline Float3 Cross(const Float3& val) {
			return Float3(Y*val.Z - val.Y*Z, Z*val.X - val.Z * X, X*val.Y - val.X*Y);
		}
		// length of the vector //
		DLLEXPORT inline float Length() const{
			return sqrt(X*X+Y*Y+Z*Z);
		}
		// normalizes the vector //
		DLLEXPORT inline Float3 Normalize() const{
			const float length = Length();
			return Float3(X/length, Y/length, Z/length);
		}
		// safe version of normalization //
		DLLEXPORT inline Float3 NormalizeSafe(const Float3 &safer) const{
			// security //
			assert(safer.IsNormalized() && "safer not normalized");
			const float len = X*X+Y*Y+Z*Z;
			if(len == 0){
				return safer;
			}
			const float length = sqrt(len);
			return Float3(X/length, Y/length, Z/length);
		}
		// checks is the vector normalized //
		DLLEXPORT inline bool IsNormalized() const{
			// is absolute -1.f under normalization tolerance //
			return fabs(X*X+Y*Y+Z*Z -1.0f) < NORMALIZATION_TOLERANCE;
		}
		// does linear interpolation between vectors and coefficient f, not limited to range [0,1], courtesy of ozz-animation //
		DLLEXPORT inline Float3 Lerp(const Float3 &other, float f) const{
			return Float3((other.X-X)*f + X, (other.Y-Y)*f + Y, (other.Z-Z)*f + Z);
		}
		// compares distance between vectors to tolerance, returns true if less //
		DLLEXPORT inline bool Compare(const Float3 &other, float tolerance) const{
			const Float3 difference = (*this)-other;
			return difference.Dot(difference) < tolerance*tolerance;
		}
		// ------------------------------------ //
		// functions to be compatible with ozz functions //
		// all zero values object //
		DLLEXPORT inline static Float3 zero(){
			return Float3(0.f, 0.f, 0.f);
		}
		// all ones //
		DLLEXPORT inline static Float3 one(){
			return Float3(1.f, 1.f, 1.f);
		}
		// unitary vectors //
		// x axis
		DLLEXPORT inline static Float3 x_axis(){
			return Float3(1.f, 0.f, 0.f);
		}

		// y axis
		DLLEXPORT inline static Float3 y_axis(){
			return Float3(0.f, 1.f, 0.f);
		}

		// z axis
		DLLEXPORT inline static Float3 z_axis(){
			return Float3(0.f, 0.f, 1.f);
		}
		// ----------------- casts ------------------- //
		DLLEXPORT inline operator D3DXVECTOR3(){
			return D3DXVECTOR3(X, Y, Z);
		}
		

		// ------------------------------------ //


		float X, Y, Z;



	};
	struct Float4
	{
	public:
		DLLEXPORT Float4(){};
		DLLEXPORT Float4(float f1, float f2, float f3, float f4){
			X = f1;
			Y = f2;
			Z = f3;
			W = f4;
		}
		DLLEXPORT Float4(Float2 floats, float f3, float f4){
			X = floats.X;
			Y = floats.Y;
			Z = f3;
			W = f4;
		}
		DLLEXPORT Float4(Float3 floats, float f4){
			X = floats.X;
			Y = floats.Y;
			Z = floats.Z;
			W = f4;
		}
		DLLEXPORT explicit Float4(float val){
			X = Y = Z = val;
		}

		// access operator //
		DLLEXPORT inline float& operator[](const int &nindex){
			switch (nindex){
			case 0: return X;
			case 1: return Y;
			case 2: return Z;
			case 3: return W;
				// this should NEVER be hit //
			default: __assume(0);
			}
		}

		//************************************
		// Method:    operator float*
		// FullName:  Leviathan::Float4::operator float*
		// Access:    public 
		// Returns:   float*
		// Qualifier:
		// Usage: return first value of {X, Y, Z, W} as pointer, should be confirmed to work. D3DXMath10 has vector class that has this cast (implementation could be loaned)
		//************************************
		DLLEXPORT inline operator float* (){
			// this should be always confirmed to work //
			return &X;
		}

		// ------------------- Operators ----------------- //
		// add elements //
		DLLEXPORT inline Float4 operator +(const Float4 &val) const{
			return Float4(X+val.X, Y+val.Y, Z+val.Z, W+val.W);
		}
		// subtracts all elements //
		DLLEXPORT inline Float4 operator-(const Float4 &val) const{
			return Float4(X-val.X, Y-val.Y, Z-val.Z, W-val.W);
		}
		// negates all elements //
		DLLEXPORT inline Float4 operator-() const{
			return Float4(-X, -Y, -Z, -W);
		}
		// multiplies elements together //
		DLLEXPORT inline Float4 operator*(const Float4 &val) const{
			return Float4(X*val.X, Y*val.Y, Z*val.Z, W*val.W);
		}
		// multiply  by scalar f //
		DLLEXPORT inline Float4 operator*(float f) const{
			return Float4(X*f, Y*f, Z*f, W*f);
		}
		// divides all elements //
		DLLEXPORT inline Float4 operator/(const Float4 &val) const{
			return Float4(X/val.X, Y/val.Y, Z/val.Z, W/val.W);
		}
		// divides by float //
		DLLEXPORT inline Float4 operator/(float f) const{
			return Float4(X/f, Y/f, Z/f, W/f);
		}
		// ---- comparison operators ---- //
		// element by element comparison with operators //
		DLLEXPORT inline bool operator <(const Float4 &other) const{
			return X < other.X && Y < other.Y && Z < other.Z && W < other.W;
		};
		DLLEXPORT inline bool operator <=(const Float4 &other) const{
			return X <= other.X && Y <= other.Y && Z <= other.Z && W <= other.W;
		};
		DLLEXPORT inline bool operator >(const Float4 &other) const{
			return X > other.X && Y > other.Y && Z > other.Z && W > other.W;
		};
		DLLEXPORT inline bool operator >=(const Float4 &other) const{
			return X >= other.X && Y >= other.Y && Z > other.Z && W >= other.W;
		};
		DLLEXPORT inline bool operator ==(const Float4 &other) const{
			return X == other.X && Y == other.Y && Z == other.Z && W == other.W;
		};
		DLLEXPORT inline bool operator !=(const Float4 &other) const{
			return X != other.X && Y != other.Y && Z != other.Z && W != other.W;
		};
		// ------------------ Functions ------------------ //
		DLLEXPORT inline float GetX() const{return X;};
		DLLEXPORT inline float GetY() const{return Y;};
		DLLEXPORT inline float GetZ() const{return Z;};
		DLLEXPORT inline float GetW() const{return W;};
		DLLEXPORT inline void SetX(const float &val){ X = val; };
		DLLEXPORT inline void SetY(const float &val){ Y= val; };
		DLLEXPORT inline void SetZ(const float &val){ Z= val; };
		DLLEXPORT inline void SetW(const float &val){ W= val; };

		// add all elements together //
		DLLEXPORT inline float HAdd() const{
			return X+Y+Z+W;
		}
		// getting min and max of objects //
		DLLEXPORT inline Float4 MinElements(const Float4 &other) const{
			return Float4(X < other.X ? X : other.X, Y < other.Y ? Y : other.Y, Z < other.Z ? Z : other.Z, W < other.W ? W : other.W);
		}
		DLLEXPORT inline Float4 MaxElements(const Float4 &other) const{
			return Float4(X > other.X ? X : other.X, Y > other.Y ? Y : other.Y, Z > other.Z ? Z : other.Z, W > other.W ? W : other.W);
		}
		// value clamping //
		DLLEXPORT inline Float4 Clamp(const Float4 &min, const Float4 &max){
			const Float4 minval = this->MinElements(max);
			return min.MaxElements(minval);
		}

		// ----------------- Vector math ------------------- //
		// dot product of the vectors //
		DLLEXPORT inline float Dot(const Float4 &val) const{
			return X*val.X+Y*val.Y+Z*val.Z+W*val.W;
		}
		// length of the vector //
		DLLEXPORT inline float Length() const{
			return sqrt(X*X+Y*Y+Z*Z+W*W);
		}
		// normalizes the vector //
		DLLEXPORT inline Float4 Normalize() const{
			const float length = Length();
			return Float4(X/length, Y/length, Z/length, W/length);
		}
		// safe version of normalization //
		DLLEXPORT inline Float4 NormalizeSafe(const Float4 &safer) const{
			// security //
			assert(safer.IsNormalized() && "safer not normalized");
			const float len = X*X+Y*Y+Z*Z+W*W;
			if(len == 0){
				return safer;
			}
			const float length = sqrt(len);
			return Float4(X/length, Y/length, Z/length, W/length);
		}
		// checks is the vector normalized //
		DLLEXPORT inline bool IsNormalized() const{
			// is absolute -1.f under normalization tolerance //
			return fabs(X*X+Y*Y+Z*Z+W*W -1.0f) < NORMALIZATION_TOLERANCE;
		}
		// does linear interpolation between vectors and coefficient f, not limited to range [0,1], courtesy of ozz-animation //
		DLLEXPORT inline Float4 Lerp(const Float4 &other, float f) const{
			return Float4((other.X-X)*f + X, (other.Y-Y)*f + Y, (other.Z-Z)*f + Z, (other.W-W)*f + W);
		}
		// compares distance between vectors to tolerance, returns true if less //
		DLLEXPORT inline bool Compare(const Float4 &other, float tolerance) const{
			const Float4 difference = (*this)-other;
			return difference.Dot(difference) < tolerance*tolerance;
		}
		// ------------------------------------ //
		// All zeros //
		DLLEXPORT inline static Float4 zero() {
			return Float4(0.f, 0.f, 0.f, 0.f);
		}

		// all ones //
		DLLEXPORT inline static Float4 one() {
			return Float4(1.f, 1.f, 1.f, 1.f);
		}
		// unitary vectors for ozz support //
		// x
		DLLEXPORT inline static Float4 x_axis() {
			return Float4(1.f, 0.f, 0.f, 0.f);
		}
		// y
		DLLEXPORT inline static Float4 y_axis() {
			return Float4(0.f, 1.f, 0.f, 0.f);
		}
		// z
		DLLEXPORT inline static Float4 z_axis() {
			return Float4(0.f, 0.f, 1.f, 0.f);
		}
		// w
		DLLEXPORT inline static Float4 w_axis() {
			return Float4(0.f, 0.f, 0.f, 1.f);
		}
		// ----------------- casts ------------------- //
		DLLEXPORT inline operator D3DXVECTOR4(){
			return D3DXVECTOR4(X, Y, Z, W);
		}

		// ------------------------------------ //

		float X, Y, Z, W;
	};
}
#endif