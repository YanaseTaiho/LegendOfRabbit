#ifndef __EffekseerSystem__H__
#define __EffekseerSystem__H__

#include "FrameWork/Common.h"
#include "EffekseerManager.h"

namespace MyDirectX
{
	class EffekseerSystem : public MonoBehaviour
	{
	private:
		friend cereal::access;
		template<class Archive>
		void save(Archive & archive, std::uint32_t const version) const
		{
			archive(cereal::base_class<MonoBehaviour>(this));
			archive(type, hasStart, endDestroy, loop);
		}

		template<class Archive>
		void load(Archive & archive, std::uint32_t const version)
		{
			archive(cereal::base_class<MonoBehaviour>(this));
			archive(type, hasStart, endDestroy, loop);
		}

		void DrawImGui(int id) override;

	public:
		EffekseerSystem() {};
		~EffekseerSystem() {};

		EffekseerType type;
		bool hasStart = true;		// 実行時にエフェクトを開始するか
		bool endDestroy = false;	// エフェクト終了時にオブジェクトを削除するか
		bool loop = false;

		void Play();
		void Stop();

		Effekseer::Handle GetHandle() { return handle; }
		void EndEffect();		// 自身のエフェクトが終了時に呼ばれる

	private:
		Effekseer::Handle handle;

		void SetTransform();

		void Start() override;
		void LateUpdate() override;
		void OnDestroy() override;
	};
}

CEREAL_CLASS_VERSION(MyDirectX::EffekseerSystem, 0)
CEREAL_REGISTER_TYPE(MyDirectX::EffekseerSystem)

#endif // !__EffekseerSystem__H__