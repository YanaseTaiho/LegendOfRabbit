#ifndef _ANIMATIONCONTROLLER_H_
#define _ANIMATIONCONTROLLER_H_

#include "AnimationState.h"
#include "AnimationTransition.h"
#include "AnimationFilter.h"
#include <unordered_map>
#include <functional>

namespace FrameWork
{
	class AnimationController
	{
	private:
		friend cereal::access;
		template<class Archive>
		void save(Archive & archive, std::uint32_t const version) const
		{
			if (version >= 1)
			{
				archive(animationFilters, parameterFloatMap, parameterIntMap, parameterBoolMap, parameterTriggerMap);
			}
			else
			{
				std::vector<std::shared_ptr<AnimationTransition>> transitions;
				std::vector<std::shared_ptr<AnimationState>> animStates;
				std::weak_ptr<AnimationState> runningState;
				std::weak_ptr<AnimationTransition> runningTransition;
				std::vector < std::weak_ptr<AnimationTransition>> currentTransitions;
				archive(CEREAL_NVP(transitions), CEREAL_NVP(animStates),
					CEREAL_NVP(runningState), CEREAL_NVP(runningTransition), CEREAL_NVP(currentTransitions),
					CEREAL_NVP(parameterFloatMap), CEREAL_NVP(parameterIntMap), CEREAL_NVP(parameterBoolMap), CEREAL_NVP(parameterTriggerMap));
			}
		}
		template<class Archive>
		void load(Archive & archive, std::uint32_t const version)
		{
			if (version >= 1)
			{
				archive(animationFilters, parameterFloatMap, parameterIntMap, parameterBoolMap, parameterTriggerMap);
			}
			else
			{
				std::vector<std::shared_ptr<AnimationTransition>> transitions;
				std::vector<std::shared_ptr<AnimationState>> animStates;
				std::weak_ptr<AnimationState> runningState;
				std::weak_ptr<AnimationTransition> runningTransition;
				std::vector < std::weak_ptr<AnimationTransition>> currentTransitions;
				archive(CEREAL_NVP(transitions), CEREAL_NVP(animStates),
					CEREAL_NVP(runningState), CEREAL_NVP(runningTransition), CEREAL_NVP(currentTransitions),
					CEREAL_NVP(parameterFloatMap), CEREAL_NVP(parameterIntMap), CEREAL_NVP(parameterBoolMap), CEREAL_NVP(parameterTriggerMap));
			}
		}

	public:
		AnimationController();
		virtual ~AnimationController();

		void DrawimGui(int id);

		virtual void Initialize();
		void Start();
		void Update(Transform * const transform);

		void SetAnimationCallBack(std::string name, int frame, std::function<void(void)> callBack);

		void SetAnimation(std::string name);
		void SetParameterFloat(std::string name, float param);
		void SetParameterInt(std::string name, int param);
		void SetParameterBool(std::string name, bool param);
		void SetParameterTrigger(std::string name);

		float GetParameterFloat(std::string name);
		int GetParameterInt(std::string name);
		bool GetParameterBool(std::string name);
		bool GetParameterTrigger(std::string name);

		void ResetParameterTrigger();

		// 現在のアニメーションの進行度( 0.0f ～　1.0f )
		float GetCurrentPercent(int layer = 0);
		bool IsCurrentState(std::string name);
	protected:
		void AddFilter(std::string name, std::function<void(std::shared_ptr<AnimationFilter> & filter)> func);

		std::weak_ptr<float> AddParameterFloat(std::string name, float param = 0.0f);
		std::weak_ptr<int> AddParameterInt(std::string name, int param = 0);
		std::weak_ptr<bool> AddParameterBool(std::string name, bool param = false);
		std::weak_ptr<bool> AddParameterTrigger(std::string name, bool param = false);

	private:
		std::vector<std::shared_ptr<AnimationFilter>> animationFilters;	// 状態遷移のインスタンスリスト

		// パラメータのデータ格納用
		std::unordered_map<std::string, std::shared_ptr<float>> parameterFloatMap;
		std::unordered_map<std::string, std::shared_ptr<int>> parameterIntMap;
		std::unordered_map<std::string, std::shared_ptr<bool>> parameterBoolMap;
		std::unordered_map<std::string, std::shared_ptr<bool>> parameterTriggerMap;
	};
}

CEREAL_CLASS_VERSION(FrameWork::AnimationController, 1)

#endif // !_ANIMATIONCONTROLLER_H_

