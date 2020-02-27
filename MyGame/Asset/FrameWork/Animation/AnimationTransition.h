#ifndef _ANIMATIONTRANSITION_H_
#define _ANIMATIONTRANSITION_H_

#include "AnimationState.h"

namespace FrameWork
{
	enum Condition
	{
		Greater,	// �`�ȏ�
		Less		// �`�ȉ�
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

				if (version >= 2) archive(isAuto);
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

		void SetOption(float duration, bool isHasExitTime, float exitTime = 1.0f, bool isAuto = true);
		void OnStart();

		// �p�����[�^���ݒ肵���l�̏����𖞂����� �^
		void AddConditionFloat(std::weak_ptr<float> parameter, Condition condition, float conditionFloat);
		// �p�����[�^���ݒ肵���l�ƈ�v����� �^
		void AddConditionInt(std::weak_ptr<int> parameter, int conditionInt);
		// �p�����[�^���ݒ肵���l�ƈ�v����� �^
		void AddConditionBool(std::weak_ptr<bool> parameter, bool conditionBool);
		// �p�����[�^���^�̎� �^
		void AddConditionTrigger(std::weak_ptr<bool> parameter);

		bool Update(Transform * const transform, const AnimationState * entry);

		// ��ԑJ�ڊJ�n�ł��邩�̊m�F
		bool CheckTransition(const AnimationState * entry);

		bool isHasExitTime;
		bool isAuto;	// �����ŏ������画�f���đJ�ڂ���̂�
		float exitTime;	
		float duration;	// �J�ڎ���

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

		std::vector<IntParameter> conditionInts;		// ����int�^���X�g
		std::vector<FloatParameter> conditionFloats;	// ����float�^���X�g
		std::vector<BoolParameter> conditionBools;		// ����bool�^���X�g
		std::vector<std::weak_ptr<bool>> conditionTriggers;	// ����trigger�^���X�g
	};
}

CEREAL_CLASS_VERSION(FrameWork::AnimationTransition, 2)

#endif // !_ANIMATIONTRANSITION_H_

