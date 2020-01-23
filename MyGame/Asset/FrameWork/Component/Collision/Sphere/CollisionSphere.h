#ifndef _COLLISIONSPHERE_H_
#define _COLLISIONSPHERE_H_

#include "../Collision.h"

namespace FrameWork
{
	class CollisionSphere : public Collision
	{
		friend class CollisionJudge;

	private:
		friend cereal::access;
		template<class Archive>
		void save(Archive & archive, std::uint32_t const version) const
		{
			archive(cereal::make_nvp("Collision", cereal::base_class<Collision>(this)), CEREAL_NVP(radius));
		}
		template<class Archive>
		void load(Archive & archive, std::uint32_t const version)
		{
			archive(cereal::make_nvp("Collision", cereal::base_class<Collision>(this)), CEREAL_NVP(radius));
		}

	public:
		CollisionSphere() {}
		CollisionSphere(Vector3 offset, float radius, bool isTrigger);
		CollisionSphere(std::string name, Vector3 offset, float radius, bool isTrigger);
		~CollisionSphere();

		void SetRadius(float radius);
		float GetRadius();

	private:

		float radius;

		void Awake() override;
		void DrawImGui(int id) override;
		void Update() override;
		void Draw() override;

		bool CollisionJudge(Collision * other) override;

		bool vsCollisionSphere(CollisionSphere * other) override;
		bool vsCollisionCapsule(CollisionCapsule * other) override;
		bool vsCollisionBox(CollisionBox * other) override;
		bool vsCollisionMesh(CollisionMesh * other) override;
	};
}

CEREAL_CLASS_VERSION(FrameWork::CollisionSphere, 0)
CEREAL_REGISTER_TYPE(FrameWork::CollisionSphere)
CEREAL_REGISTER_POLYMORPHIC_RELATION(FrameWork::Component, FrameWork::CollisionSphere)


#endif // !_COLLISIONSPHERE_H_

