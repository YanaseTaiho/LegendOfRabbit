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
		bool ChangeAnimation(std::string name);	// �g�����W�V�����ɉ����Đ؂�ւ�����

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

		std::weak_ptr<AnimationFilter> myFilter;						// ���g�̃t�B���^�[�Q��
		std::weak_ptr<AnimationFilter> parentFilter;					// �e�̃t�B���^�[�Q��
		std::weak_ptr<AnimationState> runningState;						// ���s���̏��
		std::weak_ptr<AnimationTransition> runningTransition;			// ���݂̏�ԑJ�ڃ��X�g

	private:
		std::vector<std::shared_ptr<AnimationFilter>> childFilters;		// �q�t�B���^�[
		std::vector<std::shared_ptr<AnimationTransition>> transitions;	// ��ԑJ�ڂ̃C���X�^���X���X�g
		std::vector<std::shared_ptr<AnimationState>> animStates;		// ��Ԃ̃C���X�^���X���X�g
	};
}

CEREAL_CLASS_VERSION(FrameWork::AnimationFilter, 0)

#endif // !_ANIMATION_FILTER_H_

