#ifndef _COLLISION_H_
#define _COLLISION_H_

#include "../../Layer.h"
#include "../Component.h"
#include "../Transform/Transform.h"

// ダブルディスパッチ

namespace FrameWork
{
	class CollisionJudge;
	class Rigidbody;
	class CollisionSphere;
	class CollisionCapsule;
	class CollisionBox;
	class CollisionMesh;

	class Collision : public Component
	{
		friend class CollisionJudge;

	private:
		friend cereal::access;
		template<class Archive>
		void save(Archive & archive, std::uint32_t const version) const
		{
			archive(cereal::make_nvp("Component", cereal::base_class<Component>(this)),
				CEREAL_NVP(name), CEREAL_NVP(isTrigger), CEREAL_NVP(scaleRadius), 
				CEREAL_NVP(localMatrix), CEREAL_NVP(worldMatrix), CEREAL_NVP(rigidbody));
		}
		template<class Archive>
		void load(Archive & archive, std::uint32_t const version)
		{
			archive(cereal::make_nvp("Component", cereal::base_class<Component>(this)),
				CEREAL_NVP(name), CEREAL_NVP(isTrigger), CEREAL_NVP(scaleRadius),
				CEREAL_NVP(localMatrix), CEREAL_NVP(worldMatrix), CEREAL_NVP(rigidbody));
		}

	protected:
		void LoadSerialized(const std::shared_ptr<Component> & component) override;

	public:
		Collision() {};
		Collision(std::string name, bool isTrigger) : name(name), isTrigger(isTrigger), scaleRadius(0.0f) {};
		virtual ~Collision() {};
	
		
		static std::list<std::weak_ptr<Collision>> & CollisionList(int layer) { return collisionLists[layer]; }
		// リジッドボディを持っていないコリジョンリスト
		static std::list<std::weak_ptr<Collision>> & NormalCollisionList(int layer) { return normalCollisionLists[layer]; }


		void SetRigidbody(std::weak_ptr<Rigidbody> rigidbody);
		void RemoveRigidbody();

		virtual void Update() = 0;
		virtual void Draw() {};

		virtual bool CollisionJudge(Collision * other) = 0;

		virtual bool vsCollisionSphere(CollisionSphere * other) = 0;
		virtual bool vsCollisionCapsule(CollisionCapsule * other) = 0;
		virtual bool vsCollisionBox(CollisionBox * other) = 0;
		virtual bool vsCollisionMesh(CollisionMesh * other) = 0;

		std::string name;
		bool isTrigger;
		float scaleRadius;	// コリジョンの大まかな半径

		// 参照がある場合押し出しなどの計算に使う
		std::weak_ptr<Rigidbody> rigidbody;

		Matrix4 worldMatrix;
		Matrix4 localMatrix;

	protected:

		void SetScaleRadius(float scaleRadius)
		{
			this->scaleRadius = scaleRadius;
		}

		// コリジョンをワールド空間で定義する
		void ChangeWorld();

	private:
		
		void AddComponent() override;
		void RemoveComponent() override;
		void ChangeLayer(Layer layer) override;

		static std::list<std::weak_ptr<Collision>> normalCollisionLists[(int)Layer::MAX];	// リジッドボディを持っていないコリジョンをまとめておく
		static std::list<std::weak_ptr<Collision>> collisionLists[(int)Layer::MAX];
		
	};
}

CEREAL_CLASS_VERSION(FrameWork::Collision, 0)
CEREAL_REGISTER_TYPE(FrameWork::Collision)

#endif // !_COLLISION_H_

