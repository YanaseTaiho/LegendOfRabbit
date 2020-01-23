#ifndef _ANIMATIONSTATE_H_
#define _ANIMATIONSTATE_H_

#include "AnimationClip.h"

namespace FrameWork
{
	class AnimationTransition;

	class AnimationState
	{
	private:
		friend cereal::access;
		template<class Archive>
		void save(Archive & archive, std::uint32_t const version) const
		{
			archive(CEREAL_NVP(name), CEREAL_NVP(motion));
		}
		template<class Archive>
		void load(Archive & archive, std::uint32_t const version)
		{
			archive(CEREAL_NVP(name), CEREAL_NVP(motion));
			LoadSerialize();
		}

		void LoadSerialize();

	public:
		AnimationState() {}
		virtual ~AnimationState() {}

		void SetName(std::string name);
		// AnimationClipManager�ɓo�^���ꂽ���O�ƈ�v�����f�[�^���Z�b�g����( ������Ȃ������牽�����Ȃ� )
		bool SetAnimationClip(std::string animClipName, float speed, bool loop);

		void Update(Transform * transform);

		std::string name;
		std::shared_ptr<AnimationClip> motion;
	private:
	};
}

CEREAL_CLASS_VERSION(FrameWork::AnimationState, 0)

#endif // !_ANIMATIONSTATE_H_

