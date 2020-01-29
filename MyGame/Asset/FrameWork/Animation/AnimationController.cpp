#include "AnimationController.h"
#include "AnimationClipManager.h"
#include "../../DirectX/ImGui/imgui.h"

using namespace FrameWork;

AnimationController::AnimationController()
{
}

AnimationController::~AnimationController()
{
	std::unordered_map<std::string, std::shared_ptr<float>>().swap(parameterFloatMap);
	std::unordered_map<std::string, std::shared_ptr<int>>().swap(parameterIntMap);
	std::unordered_map<std::string, std::shared_ptr<bool>>().swap(parameterBoolMap);
	std::unordered_map<std::string, std::shared_ptr<bool>>().swap(parameterTriggerMap);
}

void AnimationController::DrawimGui(int id)
{
	for (auto & filter : animationFilters)
	{
		filter->DrawimGui(id);
	}
}

void AnimationController::AddFilter(std::string name, std::function<void(std::shared_ptr<AnimationFilter> & filter)> func)
{
	for (auto itr = animationFilters.begin(), end = animationFilters.end(); itr != end; ++itr)
	{
		// 既にあった場合はコピーして入れなおす
		if ((*itr)->name == name)
		{
			std::shared_ptr<AnimationFilter> oldAnim = *itr;
			animationFilters.erase(itr);

			animationFilters.emplace_back(std::make_shared<AnimationFilter>(*oldAnim));
			auto newFilter = animationFilters.back();
			newFilter->myFilter = newFilter;
			func(newFilter);
			return;
		}
	}

	animationFilters.emplace_back(std::make_shared<AnimationFilter>());
	auto & filter = animationFilters.back();
	filter->myFilter = filter;
	filter->name = name;
	func(filter);
}

void AnimationController::Initialize()
{
	//std::vector<std::shared_ptr<AnimationFilter>>().swap(animationFilters);

	std::unordered_map<std::string, std::shared_ptr<float>>().swap(parameterFloatMap);
	std::unordered_map<std::string, std::shared_ptr<int>>().swap(parameterIntMap);
	std::unordered_map<std::string, std::shared_ptr<bool>>().swap(parameterBoolMap);
	std::unordered_map<std::string, std::shared_ptr<bool>>().swap(parameterTriggerMap);
}

void AnimationController::Start()
{
	for (auto & filter : animationFilters)
	{
		filter->Start();
	}
}

void AnimationController::Update(Transform * const transform)
{
	bool isTrasition = false;
	// ここで次に遷移する条件を満たしたら遷移状態に入る
	for (auto & filter : animationFilters)
	{
		if (filter->runningState.expired()) continue;

		// 遷移中の時は次のステートの条件を確認する
		if (!filter->runningTransition.expired())
		{
			if (filter->runningTransition.lock()->nextAnimation.lock()->CheckTransition())
			{
				isTrasition = true;
			}
		}
		// 現在のステートの位置から親を辿って確認する
		else
		{
			if (filter->runningState.lock()->CheckTransition())
			{
				isTrasition = true;
			}
		}

		filter->Update(transform);
	}
	if(isTrasition) 
		ResetParameterTrigger();
}

void AnimationController::SetAnimationCallBack(std::string name, int frame, std::function<void(void)> callBack)
{
	for (auto filter : animationFilters)
	{
		if (filter->SetAnimationCallBack(name, frame, callBack)) return;
	}
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
	for (auto & filter : animationFilters)
	{
		if (filter->SetAnimation(name)) return;
	}
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

float AnimationController::GetParameterFloat(std::string name)
{
	if (parameterFloatMap.count(name) == 0) return 0.0f;

	return *parameterFloatMap[name];
}

int AnimationController::GetParameterInt(std::string name)
{
	if (parameterIntMap.count(name) == 0) return 0;

	return *parameterIntMap[name];
}

bool AnimationController::GetParameterBool(std::string name)
{
	if (parameterBoolMap.count(name) == 0) return false;

	return *parameterBoolMap[name];
}

bool AnimationController::GetParameterTrigger(std::string name)
{
	if (parameterTriggerMap.count(name) == 0) return false;

	return *parameterTriggerMap[name];
}

void AnimationController::ResetParameterTrigger()
{
	// トリガーパラメータだけ状態遷移成功時リセット
	for (auto & param : parameterTriggerMap)
	{
		*param.second = false;
	}
}

float AnimationController::GetCurrentPercent(int layer)
{
	for (auto & filter : animationFilters)
	{
		if (layer == 0)
		{
			if (filter->runningState.expired()) break;

			return filter->runningState.lock()->motion->GetCurrentPercent();
		}
		layer--;
	}
	return 0.0f;
}

bool AnimationController::IsCurrentState(std::string name)
{
	for (auto & filter : animationFilters)
	{
		if (filter->runningState.expired()) continue;

		if (!filter->runningTransition.expired())
		{
			if (filter->runningTransition.lock()->nextAnimation.lock()->name == name)
			{
				return true;
			}
			if (filter->runningTransition.lock()->nextAnimation.lock()->animFilter.lock()->name == name)
			{
				return true;
			}
		}
		if (filter->runningState.lock()->name == name || filter->runningState.lock()->animFilter.lock()->name == name)
		{
			return true;
		}
	}

	return false;
}
