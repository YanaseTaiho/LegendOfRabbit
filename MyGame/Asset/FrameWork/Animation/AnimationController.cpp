#include "AnimationController.h"
#include "AnimationClipManager.h"
#include "../../DirectX/ImGui/imgui.h"

using namespace FrameWork;

AnimationController::AnimationController()
{
}

AnimationController::~AnimationController()
{
	std::vector<std::shared_ptr<AnimationState>>().swap(animStates);

	std::vector<std::shared_ptr<AnimationTransition>>().swap(transitions);
	std::vector<std::weak_ptr<AnimationTransition>>().swap(currentTransitions);

	std::unordered_map<std::string, std::shared_ptr<float>>().swap(parameterFloatMap);
	std::unordered_map<std::string, std::shared_ptr<int>>().swap(parameterIntMap);
	std::unordered_map<std::string, std::shared_ptr<bool>>().swap(parameterBoolMap);
	std::unordered_map<std::string, std::shared_ptr<bool>>().swap(parameterTriggerMap);
}

void AnimationController::DrawimGui(int id)
{
	int cnt = id;
	for (auto & anim : animStates)
	{
		std::string strId = "##" + std::to_string(cnt);
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

std::weak_ptr<AnimationState> AnimationController::AddState(std::string name)
{
	for (auto itr = animStates.begin(), end = animStates.end(); itr != end; ++itr)
	{
		// 既にあった場合はコピーして入れなおす
		if ((*itr)->name == name)
		{
			std::shared_ptr<AnimationState> oldAnim = *itr;
			animStates.erase(itr);

			animStates.emplace_back(std::make_shared<AnimationState>(*oldAnim));
			auto newState = animStates.back();

			// トランジションの更新
			/*for (auto tran : transitions)
			{
				if (tran->entryAnimation.lock() == oldAnim)
					tran->entryAnimation = newState;

				if (tran->nextAnimation.lock() == oldAnim)
					tran->nextAnimation = newState;
			}*/
			
			return newState;
		}
	}

	animStates.emplace_back(std::make_shared<AnimationState>());
	animStates.back()->SetName(name);

	return animStates.back();
}

void AnimationController::Initialize()
{
	std::vector<std::shared_ptr<AnimationTransition>>().swap(transitions);
	std::vector<std::weak_ptr<AnimationTransition>>().swap(currentTransitions);

	std::unordered_map<std::string, std::shared_ptr<float>>().swap(parameterFloatMap);
	std::unordered_map<std::string, std::shared_ptr<int>>().swap(parameterIntMap);
	std::unordered_map<std::string, std::shared_ptr<bool>>().swap(parameterBoolMap);
	std::unordered_map<std::string, std::shared_ptr<bool>>().swap(parameterTriggerMap);
}

void AnimationController::Start()
{
	for (auto animState : animStates)
	{
		if (!animState->motion->IsTakeNode())
		{
			auto newTakeNode = Singleton<AnimationClipManager>::Instance()->GetTakeNode(animState->motion->GetName());
			animState->motion->SetTakeNode(newTakeNode);
		}
	}

	SetCurrentTransitions(runningState);
}

void AnimationController::Update(Transform * transform)
{
	if (runningState.expired()) return;

	// ここで次に遷移する条件を満たしたら遷移状態に入る
	for (auto & transition : currentTransitions)
	{
		// 状態遷移の条件を確認
		if (transition.lock()->CheckTransition())
		{
			// 既に遷移中かどうか確かめる
			bool isTransition = !runningTransition.expired();

			// 次の状態に強制的に移行
			if (isTransition)
			{
				runningState = runningTransition.lock()->nextAnimation;
				runningState.lock()->motion->OnStart();	// フレームを初期化
				SetCurrentTransitions(runningState);
			}
			else
			{
				runningTransition = transition;
				runningTransition.lock()->OnStart();
				SetCurrentTransitions(transition.lock()->entryAnimation);
			}

			// トリガーパラメータだけ状態遷移成功時リセット
			for (auto & param : parameterTriggerMap)
			{
				*param.second = false;
			}

			break;
		}
	}

	// 状態更新
	if (runningTransition.expired())
	{
		runningState.lock()->Update(transform);
	}
	// 状態遷移更新
	else
	{
		// 遷移が完了したらtrueが帰る
		if (runningTransition.lock()->Update(transform))
		{
			// 次の状態に移行
			runningState = runningTransition.lock()->nextAnimation;
			runningTransition.reset();

			SetCurrentTransitions(runningState);
		}
	}
}

void AnimationController::SetAnimationCallBack(std::string name, int frame, std::function<void(void)> callBack)
{
	for (auto anim : animStates)
	{
		if (anim->name == name && anim->motion)
		{
			anim->motion->SetCallBackFrame(frame, callBack);
			return;
		}
	}
}

std::weak_ptr<AnimationState> AnimationController::GetState(std::string name)
{
	for (auto & state : animStates)
	{
		if (state->name == name)
		{
			return state;
		}
	}

	return std::weak_ptr<AnimationState>();
}

std::weak_ptr<AnimationTransition> AnimationController::AddTransition(std::weak_ptr<AnimationState> entryState, std::weak_ptr<AnimationState> nextState)
{
	if (entryState.expired() || nextState.expired()) 
		return std::weak_ptr<AnimationTransition>();

	//for (auto itr = transitions.begin(), end = transitions.end(); itr != end; ++itr)
	//{
	//	// 既にあった場合はコピーして入れなおす
	//	if ((*itr)->entryAnimation.lock() == entryState.lock() && (*itr)->nextAnimation.lock() == nextState.lock())
	//	{
	//		std::shared_ptr<AnimationTransition> oldTrans = *itr;
	//		transitions.erase(itr);

	//		transitions.emplace_back(std::make_shared<AnimationTransition>(*oldTrans));
	//		return transitions.back();
	//	}
	//}

	transitions.emplace_back(std::make_shared<AnimationTransition>(entryState, nextState));
	return transitions.back();
}

std::weak_ptr<AnimationTransition> AnimationController::AddTransition(std::weak_ptr<AnimationState> entryState, std::weak_ptr<AnimationState> nextState, std::function<void(std::shared_ptr<AnimationTransition> & transition)> func)
{
	if (entryState.expired() || nextState.expired())
		return std::weak_ptr<AnimationTransition>();

	//for (auto itr = transitions.begin(), end = transitions.end(); itr != end; ++itr)
	//{
	//	// 既にあった場合はコピーして入れなおす
	//	if ((*itr)->entryAnimation.lock() == entryState.lock() && (*itr)->nextAnimation.lock() == nextState.lock())
	//	{
	//		std::shared_ptr<AnimationTransition> oldTrans = *itr;
	//		transitions.erase(itr);

	//		transitions.emplace_back(std::make_shared<AnimationTransition>(*oldTrans));
	//		func(transitions.back());
	//		return transitions.back();
	//	}
	//}

	transitions.emplace_back(std::make_shared<AnimationTransition>(entryState, nextState));
	func(transitions.back());
	return transitions.back();
}

std::weak_ptr<float> AnimationController::AddParameterFloat(std::string name, float param)
{
	if (parameterFloatMap.count(name) > 0) return parameterFloatMap[name];

	parameterFloatMap[name] = std::make_shared<float>(param);
	return parameterFloatMap[name];
}

std::weak_ptr<int> AnimationController::AddParameterInt(std::string name, int param)
{
	if (parameterIntMap.count(name) > 0) return parameterIntMap[name];

	parameterIntMap[name] = std::make_shared<int>(param);
	return parameterIntMap[name];
}

std::weak_ptr<bool> AnimationController::AddParameterBool(std::string name, bool param)
{
	if (parameterBoolMap.count(name) > 0) return parameterBoolMap[name];

	parameterBoolMap[name] = std::make_shared<bool>(param);
	return parameterBoolMap[name];
}

std::weak_ptr<bool> AnimationController::AddParameterTrigger(std::string name, bool param)
{
	if (parameterTriggerMap.count(name) > 0) return parameterTriggerMap[name];

	parameterTriggerMap[name] = std::make_shared<bool>(param);
	return parameterTriggerMap[name];
}

void AnimationController::SetAnimation(std::string name)
{
	for (auto anim : animStates)
	{
		if (anim->name == name)
		{
			runningState = anim;
			break;
		}
	}

	runningState.lock()->motion->OnStart();	// フレームを初期化
	SetCurrentTransitions(runningState);
}

void AnimationController::SetParameterFloat(std::string name, float param)
{
	if (parameterFloatMap.count(name) == 0) return;

	*parameterFloatMap[name] = param;
}

void AnimationController::SetParameterInt(std::string name, int param)
{
	if (parameterIntMap.count(name) == 0) return;

	*parameterIntMap[name] = param;
}

void AnimationController::SetParameterBool(std::string name, bool param)
{
	if (parameterBoolMap.count(name) == 0) return;

	*parameterBoolMap[name] = param;
}

void AnimationController::SetParameterTrigger(std::string name)
{
	if (parameterTriggerMap.count(name) == 0) return;

	*parameterTriggerMap[name] = true;
}

float AnimationController::GetCurrentPercent()
{
	if (runningState.expired()) return 0.0f;

	return runningState.lock()->motion->GetCurrentPercent();
}

bool AnimationController::IsCurrentState(std::string name)
{
	if (this->runningState.expired()) return false;

	if (!runningTransition.expired())
	{
		return runningTransition.lock()->nextAnimation.lock()->name == name;
	}

	return this->runningState.lock()->name == name;
}

void AnimationController::SetEntryPoint(std::weak_ptr<AnimationState> entryState)
{
	this->runningState = entryState;
}

void AnimationController::SetCurrentTransitions(std::weak_ptr<AnimationState> entryState)
{
	currentTransitions.clear();

	for (auto & transition : transitions)
	{
		if (transition->entryAnimation.lock() == entryState.lock())
		{
			currentTransitions.emplace_back(transition);
		}
	}
}
