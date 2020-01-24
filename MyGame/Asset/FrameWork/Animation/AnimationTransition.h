#ifndef _ANIMATIONTRANSITION_H_
#define _ANIMATIONTRANSITION_H_

#include "AnimationState.h"

namespace FrameWork
{
	enum Condition
	{
		Greater,	// ～以上
		Less		// ～以下
	};

	class AnimationTransition
	{
	private:
		friend cereal::access;
		template<class Archive>
		void serialize(Archive & archive, std::uint32_t const version)
		{
			if (version >= 1)
			{
				archive(nextAnimation,conditionInts, conditionFloats, conditionBools, conditionTriggers);
				archive(duration, isHasExitTime, exitTime);
			}
			else
			{
				std::weak_ptr<AnimationState> entryAnimation;
				archive(entryAnimation, nextAnimation, conditionInts, conditionFloats, conditionBools, conditionTriggers);
				archive(duration, isHasExitTime, exitTime);
			}
		}

	public:
		AnimationTransition() {}
		~AnimationTransition();

		void SetOption(float duration, bool isHasExitTime, float exitTime = 1.0f);
		void OnStart();

		// パラメータが設定した値の条件を満たせば 真
		void AddConditionFloat(std::weak_ptr<float> parameter, Condition condition, float conditionFloat);
		// パラメータが設定した値と一致すれば 真
		void AddConditionInt(std::weak_ptr<int> parameter, int conditionInt);
		// パラメータが設定した値と一致すれば 真
		void AddConditionBool(std::weak_ptr<bool> parameter, bool conditionBool);
		// パラメータが真の時 真
		void AddConditionTrigger(std::weak_ptr<bool> parameter);

		bool Update(Transform * const transform, const AnimationState * entry);

		// 状態遷移開始できるかの確認
		bool CheckTransition(const AnimationState * entry);

		bool isHasExitTime;
		float exitTime;	
		float duration;	// 遷移時間

		//std::weak_ptr<AnimationState> entryAnimation;
		std::weak_ptr<AnimationState> nextAnimation;
	private:
		float time = 0.0f;

		bool CheckConditionFloats();
		bool CheckConditionInts();
		bool CheckConditionBools();
		bool CheckConditionTriggers();

		struct FloatParameter
		{
			std::weak_ptr<float> parameter;
			Condition condition;
			float conditionFloat;
		private:
			friend cereal::access;
			template<class Archive>
			void serialize(Archive & archive)
			{
				archive(CEREAL_NVP(parameter), CEREAL_NVP(condition), CEREAL_NVP(conditionFloat));
			}
		};

		struct IntParameter
		{
			std::weak_ptr<int> parameter;
			int conditionInt;
		private:
			friend cereal::access;
			template<class Archive>
			void serialize(Archive & archive)
			{
				archive(CEREAL_NVP(parameter), CEREAL_NVP(conditionInt));
			}
		};

		struct BoolParameter
		{
			std::weak_ptr<bool> parameter;
			bool conditionBool;
		private:
			friend cereal::access;
			template<class Archive>
			void serialize(Archive & archive)
			{
				archive(CEREAL_NVP(parameter), CEREAL_NVP(conditionBool));
			}
		};

		std::vector<IntParameter> conditionInts;		// 条件int型リスト
		std::vector<FloatParameter> conditionFloats;	// 条件float型リスト
		std::vector<BoolParameter> conditionBools;		// 条件bool型リスト
		std::vector<std::weak_ptr<bool>> conditionTriggers;	// 条件trigger型リスト
	};
}

CEREAL_CLASS_VERSION(FrameWork::AnimationTransition, 1)

#endif // !_ANIMATIONTRANSITION_H_

