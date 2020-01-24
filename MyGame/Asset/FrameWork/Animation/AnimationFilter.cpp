#include "AnimationFilter.h"
#include "AnimationState.h"
#include "AnimationTransition.h"
#include "AnimationClipManager.h"
#include "../../DirectX/ImGui/imgui.h"

using namespace FrameWork;

void AnimationFilter::DrawimGui(int id)
{
	int cnt = id;
	std::string strId = "##" + std::to_string(cnt);
	if (ImGui::CollapsingHeader((name + strId).c_str(), ImGuiTreeNodeFlags_DefaultOpen))
	{
		for (auto & anim : animStates)
		{
			strId = "##" + std::to_string(cnt);
			if (ImGui::TreeNode((anim->name + strId).c_str()))
			{
				Singleton<AnimationClipManager>::Instance()->DropTargetImGui(anim->motion, std::to_string(cnt));

				if (anim->motion)
				{
					anim->motion->DrawImGui(cnt);
				}
				ImGui::TreePop();
			}
			cnt++;
		}
	}

	for (auto & filter : childFilters)
	{
		filter->DrawimGui(cnt);
		cnt++;
	}
}

void AnimationFilter::Start()
{
	for (auto animState : animStates)
	{
		if (!animState->motion) continue;
		if (!animState->motion->IsTakeNode())
		{
			auto newTakeNode = Singleton<AnimationClipManager>::Instance()->GetTakeNode(animState->motion->GetName());
			animState->motion->SetTakeNode(newTakeNode);
		}
	}

	if (!runningState.expired())
	{
		runningState.lock()->OnStart();
	}
	runningTransition.reset();
}

bool AnimationFilter::SetAnimationCallBack(std::string name, int frame, std::function<void(void)> callBack)
{
	for (auto filter : childFilters)
	{
		if (filter->SetAnimationCallBack(name, frame, callBack))
		{
			return true;
		}
	}

	for (auto anim : animStates)
	{
		if (anim->name == name && anim->motion)
		{
			anim->motion->SetCallBackFrame(frame, callBack);
			return true;
		}
	}
	return false;
}

bool AnimationFilter::SetAnimation(std::string name)
{
	for (auto filter : childFilters)
	{
		if (filter->SetAnimation(name))
		{
			return true;
		}
	}

	for (auto anim : animStates)
	{
		if (anim->name == name)
		{
			runningState = anim;
			runningState.lock()->motion->OnStart();	// フレームを初期化
			runningTransition.reset();
			return true;
		}
	}

	return false;
}

void AnimationFilter::SetEntryPoint(std::weak_ptr<AnimationState> entryState)
{
	runningState = entryState;
}

std::shared_ptr<AnimationFilter> & AnimationFilter::AddFilter(std::string name, std::function<void(std::shared_ptr<AnimationFilter> & filter)> func)
{
	for (auto itr = childFilters.begin(), end = childFilters.end(); itr != end; ++itr)
	{
		// 既にあった場合はコピーして入れなおす
		if ((*itr)->name == name)
		{
			std::shared_ptr<AnimationFilter> oldAnim = *itr;
			childFilters.erase(itr);

			childFilters.emplace_back(std::make_shared<AnimationFilter>(*oldAnim));
			auto & child = childFilters.back();
			child->myFilter = child;
			child->parentFilter = myFilter;

			if (func) func(child);

			return child;
		}
	}

	childFilters.emplace_back(std::make_shared<AnimationFilter>());
	auto & child = childFilters.back();
	child->myFilter = child;
	child->parentFilter = myFilter;
	child->name = name;

	if (func) func(child);

	return child;
}

std::shared_ptr<AnimationState> & AnimationFilter::AddState(std::string name)
{
	for (auto itr = animStates.begin(), end = animStates.end(); itr != end; ++itr)
	{
		// 既にあった場合はコピーして入れなおす
		if ((*itr)->name == name)
		{
			std::shared_ptr<AnimationState> oldAnim = *itr;
			animStates.erase(itr);

			animStates.emplace_back(std::make_shared<AnimationState>(*oldAnim));
			animStates.back()->animFilter = myFilter;
			return animStates.back();
		}
	}

	animStates.emplace_back(std::make_shared<AnimationState>());
	animStates.back()->animFilter = myFilter;
	animStates.back()->name = name;
	return animStates.back();
}

void AnimationFilter::AddTransition(std::weak_ptr<AnimationState> nextState, std::function<void(std::shared_ptr<AnimationTransition>&transition)> func)
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

void AnimationFilter::AddTransition(std::weak_ptr<AnimationFilter> filter, std::function<void(std::shared_ptr<AnimationTransition> & transition)> func)
{
	for (auto state : filter.lock()->animStates)
	{
		AddTransition(state, func);
	}

	for (auto child : filter.lock()->childFilters)
	{
		AddTransition(child, func);
	}
}

void AnimationFilter::AddTransition(std::string nextStateName, std::function<void(std::shared_ptr<AnimationTransition>&transition)> func)
{
	auto state = GetState(nextStateName);
	if (state.expired()) return;

	for (auto itr = transitions.begin(), end = transitions.end(); itr != end; ++itr)
	{
		if ((*itr)->nextAnimation.expired()) continue;
		if ((*itr)->nextAnimation.lock()->name == state.lock()->name)
		{
			transitions.erase(itr);
			break;
		}
	}

	transitions.emplace_back(std::make_shared<AnimationTransition>());
	auto & add = transitions.back();
	add->nextAnimation = state;
	func(add);
}

std::weak_ptr<AnimationState> AnimationFilter::GetState(std::string name)
{
	for (auto filter : childFilters)
	{
		auto state = filter->GetState(name);
		if (!state.expired())
		{
			return state;
		}
	}

	for (auto state : animStates)
	{
		if (state->name == name)
		{
			return state;
		}
	}
	return std::weak_ptr<AnimationState>();
}

AnimationFilter * const AnimationFilter::GetBossParent()
{
	if (!parentFilter.expired())
	{
		return parentFilter.lock()->GetBossParent();
	}

	return this;
}

bool AnimationFilter::CheckTransition(const AnimationState * currentState)
{
	if (!parentFilter.expired())
	{
		if (parentFilter.lock()->CheckTransition(currentState))
		{
			return true;
		}
	}

	// ここで次に遷移する条件を満たしたら遷移状態に入る
	for (auto & transition : transitions)
	{
		// 状態遷移の条件を確認
		if (transition->CheckTransition(currentState))
		{
			auto const filter = GetBossParent();
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

void AnimationFilter::ChangeTransition(std::weak_ptr<AnimationTransition> transition)
{
	runningTransition = transition;
	runningTransition.lock()->OnStart();
}

void AnimationFilter::Update(Transform * const transform)
{
	// 状態更新
	if (runningTransition.expired())
	{
		runningState.lock()->Update(transform);
	}
	// 状態遷移更新
	else
	{
		// 遷移が完了したらtrueが帰る
		if (runningTransition.lock()->Update(transform, runningState.lock().get()))
		{
			// 次の状態に移行
			runningState = runningTransition.lock()->nextAnimation;
			runningTransition.reset();
		}
	}
}
