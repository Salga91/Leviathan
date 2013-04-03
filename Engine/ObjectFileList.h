#ifndef LEVIATHAN_OBJECTFILE_LIST
#define LEVIATHAN_OBJECTFILE_LIST
// ------------------------------------ //
#ifndef LEVIATHAN_DEFINE
#include "Define.h"
#endif
// ------------------------------------ //
// ---- includes ---- //
#include "NamedVars.h"
#include "ScriptList.h"

namespace Leviathan{

	class ObjectFileList : public Object{
	public:
		DLLEXPORT ObjectFileList::ObjectFileList();
		DLLEXPORT ObjectFileList::ObjectFileList(wstring name);
		DLLEXPORT ObjectFileList::~ObjectFileList();

		DLLEXPORT ScriptList* AllocateNewListFromData();


		wstring Name;
		NamedVars* Variables;
		vector<wstring*> Lines; // for storing plain text //
	

	};

}
#endif