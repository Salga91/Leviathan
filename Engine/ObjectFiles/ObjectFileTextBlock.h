#ifndef LEVIATHAN_OBJECTFILE_TEXTBLOCK
#define LEVIATHAN_OBJECTFILE_TEXTBLOCK
// ------------------------------------ //
#ifndef LEVIATHAN_DEFINE
#include "Define.h"
#endif
// ------------------------------------ //
// ---- includes ---- //

namespace Leviathan{


	//! \brief Interface for object file text blocks to implement
	//! \see ObjectFileListProper
	class ObjectFileTextBlock : public Object{
	public:
		
		DLLEXPORT virtual ~ObjectFileTextBlock();

		//! \brief Adds an UTF8 encoded line
		//! \note The line will be converted into wstring (utf16)
		DLLEXPORT virtual void AddTextLine(const string &line) = 0;



	protected:
		ObjectFileTextBlock();


	};


	//! \brief Implementation of ObjectFileTextBlock
	//! \see ObjectFileTextBlock
	class ObjectFileTextBlockProper : public ObjectFileTextBlock{
	public:

		DLLEXPORT ObjectFileTextBlockProper(const wstring &name);





	protected:


		wstring Name;
		std::vector<wstring*> Lines;
	};


}
#endif
