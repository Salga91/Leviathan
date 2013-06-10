#include "Include.h"
// ------------------------------------ //
#ifndef LEVIATHAN_SKELETONBONE
#include "SkeletonBone.h"
#endif
using namespace Leviathan;
using namespace Leviathan::GameObject;
// ------------------------------------ //
DLLEXPORT Leviathan::GameObject::SkeletonBone::SkeletonBone(){
	// let default allocations be //
}

DLLEXPORT Leviathan::GameObject::SkeletonBone::SkeletonBone(const wstring &name, const Float3 &position, const Float3 &direction, int group) : 
	Name(name), RestPosition(position), RestDirection(direction), AnimationPosition(RestPosition), AnimationDirection(RestDirection)
{
	// set values + initializer list //
	BoneGroup = group;
}

DLLEXPORT Leviathan::GameObject::SkeletonBone::~SkeletonBone(){

}
// ------------------------------------ //
DLLEXPORT void Leviathan::GameObject::SkeletonBone::SetName(const wstring &name){
	Name = name;
}

DLLEXPORT void Leviathan::GameObject::SkeletonBone::SetRestPosition(const Float3 &val){
	RestPosition = val;
	AnimationPosition = RestPosition;
}

DLLEXPORT void Leviathan::GameObject::SkeletonBone::SetAnimationPosition(const Float3 &val){
	AnimationPosition = val;
}

DLLEXPORT void Leviathan::GameObject::SkeletonBone::SetBoneGroup(int group){
	BoneGroup = group;
}


// ------------------------------------ //
DLLEXPORT Float3& Leviathan::GameObject::SkeletonBone::GetRestPosition(){
	return RestPosition;
}

DLLEXPORT void Leviathan::GameObject::SkeletonBone::SetPosePosition(const Float3 &pos){
	AnimationPosition = pos;
}

DLLEXPORT void Leviathan::GameObject::SkeletonBone::SetParentName(const wstring &name){
	ParentName = name;
}

DLLEXPORT void Leviathan::GameObject::SkeletonBone::SetRestDirection(const Float3 &val){
	RestDirection = val;
	AnimationDirection = RestDirection;
}

DLLEXPORT void Leviathan::GameObject::SkeletonBone::SetAnimationDirection(const Float3 &val){
	AnimationDirection = val;
}

DLLEXPORT Float3& Leviathan::GameObject::SkeletonBone::GetRestDirection(){
	return RestDirection;
}

DLLEXPORT Float3& Leviathan::GameObject::SkeletonBone::GetAnimationDirection(){
	return AnimationDirection;
}

DLLEXPORT Float3& Leviathan::GameObject::SkeletonBone::GetAnimationPosition(){
	return AnimationPosition;
}

DLLEXPORT void Leviathan::GameObject::SkeletonBone::SetParentPtr(shared_ptr<SkeletonBone> parent, shared_ptr<SkeletonBone> thisptr){
	// add parent //
	Parent = parent;
	// set this as parent's child //
	parent->AddChildren(thisptr);
}

DLLEXPORT void Leviathan::GameObject::SkeletonBone::AddChildren(shared_ptr<SkeletonBone> bone){
	Children.push_back(bone);
}








// ------------------------------------ //

// ------------------------------------ //
