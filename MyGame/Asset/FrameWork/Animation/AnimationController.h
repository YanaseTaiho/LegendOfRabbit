#ifndef _ANIMATIONCONTROLLER_H_
#define _ANIMATIONCONTROLLER_H_

#include "AnimationState.h"
#include "AnimationTransition.h"
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
			archive(CEREAL_NVP(transitions), CEREAL_NVP(animStates),
				CEREAL_NVP(runningState), CEREAL_NVP(runningTransition), CEREAL_NVP(currentTransitions),
				CEREAL_NVP(parameterFloatMap), CEREAL_NVP(parameterIntMap), CEREAL_NVP(parameterBoolMap), CEREAL_NVP(parameterTriggerMap));
		}
		template<class Archive>
		void load(Archive & archive, std::uint32_t const version)
		{
			archive(CEREAL_NVP(transitions), CEREAL_NVP(animStates), 
				CEREAL_NVP(runningState), CEREAL_NVP(runningTransition), CEREAL_NVP(currentTransitions),
				CEREAL_NVP(parameterFloatMap), CEREAL_NVP(parameterIntMap), CEREAL_NVP(parameterBoolMap), CEREAL_NVP(parameterTriggerMap));
		}

	public:
		AnimationController();
		virtual ~AnimationController();

		void DrawimGui(int id);

		virtual void Initialize();
		void Start();
		void Update(Transform * transform);

		void SetAnimationCallBack(std::string name, int frame, std::function<void(void)> callBack);

		void SetAnimation(std::string name);
		void SetParameterFloat(std::string name, float param);
		void SetParameterInt(std::string name, int param);
		void SetParameterBool(std::string name, bool param);
		void SetParameterTrigger(std::string name);

		// ���݂̃A�j���[�V�����̐i�s�x( 0.0f �`�@1.0f )
		float GetCurrentPercent();
		bool IsCurrentState(std::string name);
	protected:
		std::weak_ptr<AnimationState> AddState(std::string name);
		std::weak_ptr<AnimationState> GetState(std::string name);

		std::weak_ptr<AnimationTransition> AddTransition(std::weak_ptr<AnimationState> entryState, std::weak_ptr<AnimationState> nextState);
		std::weak_ptr<AnimationTransition> AddTransition(std::weak_ptr<AnimationState> entryState, std::weak_ptr<AnimationState> nextState, std::function<void(std::shared_ptr<AnimationTransition> & transition)> func);

		std::weak_ptr<float> AddParameterFloat(std::string name, float param = 0.0f);
		std::weak_ptr<int> AddParameterInt(std::string name, int param = 0);
		std::weak_ptr<bool> AddParameterBool(std::string name, bool param = false);
		std::weak_ptr<bool> AddParameterTrigger(std::string name, bool param = false);

		void SetEntryPoint(std::weak_ptr<AnimationState> entryState);

	private:
		std::weak_ptr<AnimationState> runningState;	// ���s���̏��
		std::weak_ptr<AnimationTransition> runningTransition;	// ���s���̏�ԑJ��
		std::vector<std::weak_ptr<AnimationTransition>> currentTransitions;	// ���݂̏�ԑJ�ڃ��X�g

		std::vector<std::shared_ptr<AnimationTransition>> transitions;	// ��ԑJ�ڂ̃C���X�^���X���X�g
		std::vector<std::shared_ptr<AnimationState>> animStates;		// ��Ԃ̃C���X�^���X���X�g

		void SetCurrentTransitions(std::weak_ptr<AnimationState> entryState);

		// �p�����[�^�̃f�[�^�i�[�p
		std::unordered_map<std::string, std::shared_ptr<float>> parameterFloatMap;
		std::unordered_map<std::string, std::shared_ptr<int>> parameterIntMap;
		std::unordered_map<std::string, std::shared_ptr<bool>> parameterBoolMap;
		std::unordered_map<std::string, std::shared_ptr<bool>> parameterTriggerMap;
	};
}

CEREAL_CLASS_VERSION(FrameWork::AnimationController, 0)

#endif // !_ANIMATIONCONTROLLER_H_

