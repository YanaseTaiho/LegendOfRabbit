#ifndef _ANIMATIONSTATE_H_
#define _ANIMATIONSTATE_H_

#include "AnimationClip.h"

namespace FrameWork
{
	class AnimationTransition;
	class AnimationFilter;

	class AnimationState
	{
	private:
		friend cereal::access;
		template<class Archive>
		void save(Archive & archive, std::uint32_t const version) const
		{
			archive(CEREAL_NVP(name), CEREAL_NVP(motion));

			if (version >= 1)
			{
				archive(animFilter, transitions);
			}
		}
		template<class Archive>
		void load(Archive & archive, std::uint32_t const version)
		{
			archive(CEREAL_NVP(name), CEREAL_NVP(motion));

			if (version >= 1)
			{
				archive(animFilter, transitions);
			}
			LoadSerialize();
		}

		void LoadSerialize();

	public:
		AnimationState() {}
		virtual ~AnimationState() {}

		void SetName(std::string name);
		// AnimationClipManager�ɓo�^���ꂽ���O�ƈ�v�����f�[�^���Z�b�g����( ������Ȃ������牽�����Ȃ� )
		bool SetAnimationClip(std::string animClipName, float speed, bool loop);
		void AddTransition(std::weak_ptr<AnimationState> nextState, std::function<void(std::shared_ptr<AnimationTransition> & transition)> func);

		void OnStart();
		bool CheckTransition();
		void Update(Transform * const transform);

		std::string name;
		std::shared_ptr<AnimationClip> motion;

		std::weak_ptr<AnimationFilter> animFilter;						// ���̃X�e�[�g�����݂���t�B���^�[�̃|�C���^
		std::vector<std::shared_ptr<AnimationTransition>> transitions;	// ��ԑJ�ڂ̃C���X�^���X���X�g
	private:
	};
}

CEREAL_CLASS_VERSION(FrameWork::AnimationState, 1)

#endif // !_ANIMATIONSTATE_H_

