#ifndef MONOBEHAVIOUR_H_
#define MONOBEHAVIOUR_H_

#include "../Component.h"

namespace FrameWork
{
	class Collision;

	class MonoBehaviour : public Component
	{
	private:
		friend cereal::access;
		template<class Archive>
		void save(Archive & archive, std::uint32_t const version) const
		{
			archive(cereal::make_nvp("Component", cereal::base_class<Component>(this)));
		}

		template<class Archive>
		void load(Archive & archive, std::uint32_t const version)
		{
			archive(cereal::make_nvp("Component", cereal::base_class<Component>(this)));
		}

		void LoadSerialized(const std::shared_ptr<Component> & component) override;

	public:
		MonoBehaviour() = default;
		virtual ~MonoBehaviour() = default;
		
		static std::list<std::weak_ptr<MonoBehaviour>> & ComponentList() { return behaviourList; }

		// 更新の最初に一度だけ呼ばれる
		virtual void Start() {}

		// 通常更新処理
		inline virtual void Update() {}
		// 遅延更新処理( その他の更新処理よりも後に呼ばれる( カメラの移動など ) )
		inline virtual void LateUpdate() {}

		// デバッグ用
		virtual void Draw() {};

		// 物理的に当たっている間は呼ばれる
		inline virtual void OnCollisionStay(std::weak_ptr<Collision> & mine, std::weak_ptr<Collision> & other) {}	 
		inline virtual void OnTriggerStay(std::weak_ptr<Collision> & mine, std::weak_ptr<Collision> & other) {}

	private:
		void AddComponent() override final;
		void RemoveComponent() override final;

		static std::list<std::weak_ptr<MonoBehaviour>> behaviourList;
	};
}

CEREAL_CLASS_VERSION(FrameWork::MonoBehaviour, 0)
CEREAL_REGISTER_TYPE(FrameWork::MonoBehaviour)

using namespace FrameWork;


#endif // !MONOBEHAVIOUR_H_

