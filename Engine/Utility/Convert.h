#pragma once
// ------------------------------------ //
#include "../Common/Types.h"
#include <sstream>

namespace Leviathan{

#define STRINGTOSOMETHINGTEMPLATEALTERNATIVE(StringyType, strstreamt, funcname, totype) \
    DLLEXPORT static inline totype funcname(const StringyType &str){ totype tempval; \
        strstreamt stream; stream.str(str.c_str()); stream >> tempval; return tempval;}
	
	//! \brief Holds common conversion functions
	class Convert{
	public:

		DLLEXPORT static double DegreesToRadians(float degrees);
		DLLEXPORT static double RadiansToDegrees(float radians);

		DLLEXPORT static int WstringFromBoolToInt(const std::wstring &i);
		DLLEXPORT static int StringFromBoolToInt(const std::string &i);

        DLLEXPORT static int StringTypeNameCheck(const std::string &name);

		DLLEXPORT static bool IsStringBool(const std::string &val, bool* receiver);

		template<class T>
		DLLEXPORT static inline T WstringTo(const std::wstring &str){
			T tempval(0);
            std::wstringstream stream;
			stream.str(str.c_str());
			stream >> tempval;
			return tempval;
		}
        
		template<class T>
		DLLEXPORT static inline T StringTo(const std::string &str){
			T tempval(0);
            std::stringstream stream;
			stream.str(str.c_str());
			stream >> tempval;
			return tempval;
		}


		// macro conversions //
		STRINGTOSOMETHINGTEMPLATEALTERNATIVE(std::wstring, std::wstringstream, WstringToInt, int);
		STRINGTOSOMETHINGTEMPLATEALTERNATIVE(std::wstring, std::wstringstream, WstringToFloat, float);

		// template functions //

		template<typename T>
		DLLEXPORT static std::wstring ToWstring(const T& val){
			std::wstringstream stream;
			if(!(stream << val)){
				return L"";
			}
			return stream.str();
		}
		template<typename T>
		DLLEXPORT static std::string ToString(const T& val){
			std::stringstream stream;
			if(!(stream << val)){
				return "";
			}
			return stream.str();
		}

		template<class T>
		DLLEXPORT static std::wstring ToHexadecimalWstring(const T& val){
			std::wstringstream stream;
			if(!(stream << std::hex << val)){
				return L"";
			}
			return stream.str();
		}

        template<class T>
		DLLEXPORT static std::string ToHexadecimalString(const T& val){
			std::stringstream stream;
			if(!(stream << std::hex << val)){
				return "";
			}
			return stream.str();
		}


		//! \brief Decodes an UTF8 string to an UTF16 string (wide string/wstring)
		//! \return The converted string or an empty string in case the input string is invalid/has invalid format
		DLLEXPORT static std::wstring Utf8ToUtf16(const std::string &utf8str);

		//! \brief Encodes an UTF8 string from a wide string (wstring/utf16)
		//! \return The converted string or an empty string in case the input string is invalid/has invalid format
		DLLEXPORT static std::string Utf16ToUtf8(const std::wstring &utf16str);


	};


}


