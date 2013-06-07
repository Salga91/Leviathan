#ifndef LEVIATHAN_ANIMATIONMANAGER
#define LEVIATHAN_ANIMATIONMANAGER
// ------------------------------------ //
#ifndef LEVIATHAN_DEFINE
#include "Define.h"
#endif
// ------------------------------------ //
// ---- includes ---- //
#include "AnimationBlock.h"
#include "LoadedAnimation.h"
#include "FileSystem.h"
#include "WstringIterator.h"
#include "ObjectFileProcessor.h"
#include "SkeletonBone.h"
#include "LineTokenizer.h"

namespace Leviathan{

	struct IndexedAnimation{
		IndexedAnimation(const wstring &name, const wstring &source) : SourceFile(source), AnimationName(name), CorrespondingAnimation(NULL){
		}
		IndexedAnimation(const wstring &source) : SourceFile(source), AnimationName(), CorrespondingAnimation(NULL){
		}

		shared_ptr<LoadedAnimation> CorrespondingAnimation;
		wstring SourceFile;
		wstring AnimationName;
	};

	class AnimationManager : public EngineComponent{
	public:
		DLLEXPORT AnimationManager::AnimationManager();
		DLLEXPORT AnimationManager::~AnimationManager();

		DLLEXPORT bool Init();
		DLLEXPORT void Release();

		// getting animations/managing //
		DLLEXPORT shared_ptr<AnimationBlock> GetAnimation(const wstring &name);
		DLLEXPORT shared_ptr<AnimationBlock> GetAnimation(int ID);
		DLLEXPORT shared_ptr<AnimationBlock> GetAnimationFromIndex(int ind);

		DLLEXPORT bool UnloadAnimation(const wstring &name);
		DLLEXPORT bool UnloadAnimation(int ID);
		DLLEXPORT bool UnloadAnimationFromIndex(int ind);


		DLLEXPORT bool IndexAllAnimations(bool LoadToMemory=false);

		DLLEXPORT static AnimationManager* Get();
	private:
		vector<shared_ptr<LoadedAnimation>> AnimationsInMemory;

		int VerifyAnimLoaded(const wstring &file, bool SkipCheck = false);
		bool IsSourceFileLoaded(const wstring &sourcefile);
		wstring GetAnimationNameFromFile(const wstring &file);
		vector<unique_ptr<IndexedAnimation>> AnimationFiles;

		// static access //
		static AnimationManager* instance;
	};

}
#endif