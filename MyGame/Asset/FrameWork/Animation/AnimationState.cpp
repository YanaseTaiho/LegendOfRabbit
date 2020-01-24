#include "AnimationState.h"
#include "AnimationFilter.h"
#include "AnimationTransition.h"
#include "AnimationClipManager.h"
#include "../Singleton.h"

using namespace FrameWork;

void AnimationState::LoadSerialize()
{
	if (!this->motion) return;

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

void AnimationState::AddTransition(std::weak_ptr<AnimationState> nextState, std::function<void(std::shared_ptr<AnimationTransition>&transition)> func)
{
	if (nextState.expired()) return;

	for (auto itr = transitions.begin(), end = transitions.end(); itr != end; ++itr)
	{
		if ((*itr)->nextAnimation.expired()) continue;
		if ((*itr)->nextAnimation.lock()->name == nextState.lock()->name)
		{
			transitions.erase(itr);
			break;
		}
	}

	transitions.emplace_back(std::make_shared<AnimationTransition>());
	auto & add = transitions.back();
	add->nextAnimation = nextState;
	func(add);
}

void AnimationState::OnStart()
{
	if(motion) motion->OnStart();
}

bool AnimationState::CheckTransition()
{
	// 先にフィルターの遷移条件を確認
	if (animFilter.lock()->CheckTransition(this))
	{
		return true;
	}

	// 次に遷移する条件を満たしたら遷移状態に入る
	for (auto & transition : transitions)
	{
		// 状態遷移の条件を確認
		if (transition->CheckTransition(this))
		{
			auto const filter = animFilter.lock()->GetBossParent();
			// 既に遷移中かどうか確かめる
			bool isTransition = !filter->runningTransition.expired();

			// 次の状態に強制的に移行
			if (isTransition)
			{
				filter->runningState = filter->runningTransition.lock()->nextAnimation;
				filter->runningState.lock()->OnStart();	// フレームを初期化
				filter->runningTransition.reset();
			}
			else
			{
				filter->ChangeTransition(transition);
			}

			return true;
		}
	}

	return false;
}

void AnimationState::Update(Transform * const transform)
{
	if (this->motion)
	{
		this->motion->UpdateAnimation(transform);
	}
}
