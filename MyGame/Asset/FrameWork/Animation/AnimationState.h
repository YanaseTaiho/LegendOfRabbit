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
		// AnimationClipManagerに登録された名前と一致したデータをセットする( 見つからなかったら何もしない )
		bool SetAnimationClip(std::string animClipName, float speed, bool loop);
		void AddTransition(std::weak_ptr<AnimationState> nextState, std::function<void(std::shared_ptr<AnimationTransition> & transition)> func);

		void OnStart();
		bool CheckTransition();
		void Update(Transform * const transform);

		std::string name;
		std::shared_ptr<AnimationClip> motion;

		std::weak_ptr<AnimationFilter> animFilter;						// このステートが存在するフィルターのポインタ
		std::vector<std::shared_ptr<AnimationTransition>> transitions;	// 状態遷移のインスタンスリスト
	private:
	};
}

CEREAL_CLASS_VERSION(FrameWork::AnimationState, 1)

#endif // !_ANIMATIONSTATE_H_

