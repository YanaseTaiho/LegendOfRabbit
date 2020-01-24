#include "AnimationTransition.h"
#include "../Time.h"

using namespace FrameWork;

AnimationTransition::~AnimationTransition()
{
	std::vector<FloatParameter>().swap(conditionFloats);
	std::vector<IntParameter>().swap(conditionInts);
	std::vector<BoolParameter>().swap(conditionBools);
	std::vector<std::weak_ptr<bool>>().swap(conditionTriggers);
}

void AnimationTransition::SetOption(float duration, bool isHasExitTime, float exitTime)
{
	this->duration = duration;
	this->isHasExitTime = isHasExitTime;
	this->exitTime = Mathf::Clamp01(exitTime);
}

void AnimationTransition::OnStart()
{
	time = 0.0f;
	nextAnimation.lock()->motion->OnStart();
}

void AnimationTransition::AddConditionFloat(std::weak_ptr<float> parameter, Condition condition, float conditionFloat)
{
	FloatParameter add;
	add.parameter = parameter;
	add.condition = condition;
	add.conditionFloat = conditionFloat;

	conditionFloats.emplace_back(add);
}

void AnimationTransition::AddConditionInt(std::weak_ptr<int> parameter, int conditionInt)
{
	IntParameter add;
	add.parameter = parameter;
	add.conditionInt = conditionInt;

	conditionInts.emplace_back(add);
}

void AnimationTransition::AddConditionBool(std::weak_ptr<bool> parameter, bool conditionBool)
{
	BoolParameter add;
	add.parameter = parameter;
	add.conditionBool = conditionBool;

	conditionBools.emplace_back(add);
}

void AnimationTransition::AddConditionTrigger(std::weak_ptr<bool> parameter)
{
	conditionTriggers.emplace_back(parameter);
}

bool AnimationTransition::Update(Transform * const transform, const AnimationState * entry)
{
	if (!nextAnimation.lock()->motion) return false;

	if (duration > 0.0f)
	{
		AnimationClip * anim1 = entry->motion.get();
		AnimationClip * anim2 = nextAnimation.lock()->motion.get();

		time += Time::DeltaTime();
		float weight = time / duration;

		weight = Mathf::Clamp01(weight);

		AnimationClip::UpdateBlendAnimation(transform, anim1, anim2, weight);

		if (weight >= 1.0f)
		{
			return true;
		}
	}
	else
	{
		nextAnimation.lock()->motion->UpdateAnimation(transform);
		return true;
	}

	return false;
}

bool AnimationTransition::CheckTransition(const AnimationState * entry)
{
	bool isExit = true;

	if (isHasExitTime)
	{
		isExit = (exitTime <= entry->motion->GetCurrentPercent());
	}

	if (!isExit) return false;

	// ここから各パラメータのチェック
	if (!CheckConditionTriggers())
		return false;
	if (!CheckConditionBools())
		return false;
	if (!CheckConditionInts())
		return false;
	if (!CheckConditionFloats())
		return false;

	return true;
}

bool AnimationTransition::CheckConditionFloats()
{
	size_t size = conditionFloats.size();
	if (size == 0) return true;
	
	for (size_t i = 0; i < size; i++)
	{
		FloatParameter conFloat = conditionFloats[i];
		float parameter = *conFloat.parameter.lock();
		float conditionParam = conFloat.conditionFloat;
		Condition condition = conFloat.condition;

		switch (condition)
		{
		case Greater:
			if (parameter < conditionParam)
			{
				return false;
			}
			break;
		case Less:
			if (parameter > conditionParam)
			{
				return false;
			}
			break;
		}
	}

	return true;
}

bool AnimationTransition::CheckConditionInts()
{
	size_t size = conditionInts.size();
	if (size == 0) return true;

	for (size_t i = 0; i < size; i++)
	{
		IntParameter conInt = conditionInts[i];
		int parameter = *conInt.parameter.lock();
		int condition = conInt.conditionInt;
		if (parameter != condition)
		{
			return false;
		}
	}

	return true;
}

bool AnimationTransition::CheckConditionBools()
{
	size_t size = conditionBools.size();
	if (size == 0) return true;

	for (size_t i = 0; i < size; i++)
	{
		BoolParameter conBool = conditionBools[i];
		bool parameter = *conBool.parameter.lock();
		bool condition = conBool.conditionBool;
		if (parameter != condition)
		{
			return false;
		}
	}

	return true;
}

bool AnimationTransition::CheckConditionTriggers()
{
	size_t size = conditionTriggers.size();
	if (size == 0) return true;

	for (size_t i = 0; i < size; i++)
	{
		bool trigger = *conditionTriggers[i].lock();
		if (!trigger)
		{
			return false;
		}
	}

	return true;
}
