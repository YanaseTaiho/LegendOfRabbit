#include "AnimationState.h"
#include "AnimationClipManager.h"
#include "../Singleton.h"

using namespace FrameWork;

void AnimationState::LoadSerialize()
{
	auto take = Singleton<AnimationClipManager>::Instance()->GetTakeNode(this->motion->GetName());
	this->motion->SetTakeNode(take);
}

void AnimationState::SetName(std::string name)
{
	this->name = name;
}

bool AnimationState::SetAnimationClip(std::string animClipName, float speed, bool loop)
{
	AnimationClip * anim = Singleton<AnimationClipManager>::Instance()->GetClone(animClipName);
	if (!anim) return false;

	this->motion = std::shared_ptr<AnimationClip>(anim);
	this->motion->SetSpeed(speed);
	this->motion->SetLoop(loop);
	return true;
}

void AnimationState::Update(Transform * transform)
{
	if (this->motion)
		this->motion->UpdateAnimation(transform);
}
