#ifndef _ANIMATION_FILTER_H_
#define _ANIMATION_FILTER_H_

#include  "../../Cereal/Common.h"

namespace FrameWork
{
	class Transform;
	class AnimationTransition;
	class AnimationState;
	class AnimationController;

	class AnimationFilter
	{
	private:
		friend cereal::access;
		template<class Archive>
		void save(Archive & archive, std::uint32_t const version) const
		{
			archive(name, myFilter, parentFilter, runningState, childFilters, transitions, animStates);
		}
		template<class Archive>
		void load(Archive & archive, std::uint32_t const version)
		{
			archive(name, myFilter, parentFilter, runningState, childFilters, transitions, animStates);

			transitions.clear();
		}

	public:
		AnimationFilter() {}
		virtual ~AnimationFilter() {}

		void DrawimGui(int id);
		void Start();

		bool SetAnimationCallBack(std::string name, int frame, std::function<void(void)> callBack);
		bool SetAnimation(std::string name);

		void SetEntryPoint(std::weak_ptr<AnimationState> entryState);
		std::shared_ptr<AnimationFilter> & AddFilter(std::string name, std::function<void(std::shared_ptr<AnimationFilter> & filter)> func = nullptr);
		std::shared_ptr<AnimationState> & AddState(std::string name);
		void AddTransition(std::weak_ptr<AnimationState> nextState, std::function<void(std::shared_ptr<AnimationTransition> & transition)> func);
		void AddTransition(std::weak_ptr<AnimationFilter> filter, std::function<void(std::shared_ptr<AnimationTransition> & transition)> func);
		void AddTransition(std::string nextStateName, std::function<void(std::shared_ptr<AnimationTransition> & transition)> func);

		std::weak_ptr<AnimationState> GetState(std::string name);

		AnimationFilter * const GetBossParent();
		bool CheckTransition(const AnimationState * currentState);
		void ChangeTransition(std::weak_ptr<AnimationTransition> transition);
		void Update(Transform * const transform);

		std::string name;

		std::weak_ptr<AnimationFilter> myFilter;						// 自身のフィルター参照
		std::weak_ptr<AnimationFilter> parentFilter;					// 親のフィルター参照
		std::weak_ptr<AnimationState> runningState;						// 実行中の状態
		std::weak_ptr<AnimationTransition> runningTransition;			// 現在の状態遷移リスト

	private:
		std::vector<std::shared_ptr<AnimationFilter>> childFilters;		// 子フィルター
		std::vector<std::shared_ptr<AnimationTransition>> transitions;	// 状態遷移のインスタンスリスト
		std::vector<std::shared_ptr<AnimationState>> animStates;		// 状態のインスタンスリスト
	};
}

CEREAL_CLASS_VERSION(FrameWork::AnimationFilter, 0)

#endif // !_ANIMATION_FILTER_H_

