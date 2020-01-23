#ifndef _COLLISIONMESH_H_
#define _COLLISIONMESH_H_

#include "../Collision.h"
#include "CollisionMeshInfo.h"

namespace FrameWork
{
	class CollisionMesh : public Collision
	{
	private:
		friend cereal::access;
		template <typename Archive>
		void save(Archive & archive) const
		{
			archive(cereal::make_nvp("Collision", cereal::base_class<Collision>(this))/*, CEREAL_NVP(meshInfo)*/);
		}
		template <typename Archive>
		void load(Archive & archive)
		{
			archive(cereal::make_nvp("Collision", cereal::base_class<Collision>(this))/*, CEREAL_NVP(meshInfo)*/);
		}

		void LoadSerialized(const std::shared_ptr<Component> & component) override;

		void DrawImGui(int id) override;

	public:
		CollisionMesh() {}
		CollisionMesh(bool isTrigger) : Collision("", isTrigger) {}
		~CollisionMesh() {}

		std::weak_ptr<CollisionMeshInfo> meshInfo;
	private:

		void Awake() override;
		void Update() override;
		void Draw() override;

		bool CollisionJudge(Collision * other) override;

		bool vsCollisionSphere(CollisionSphere * other) override;
		bool vsCollisionCapsule(CollisionCapsule * other) override;
		bool vsCollisionBox(CollisionBox * other) override;
		bool vsCollisionMesh(CollisionMesh * other) override;
	};
}

CEREAL_REGISTER_TYPE(FrameWork::CollisionMesh)

#endif // !_COLLISIONMESH_H_

