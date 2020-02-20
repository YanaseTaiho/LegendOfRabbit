#ifndef _COLLISIONSPHERE_H_
#define _COLLISIONSPHERE_H_

#include "../Collision.h"
//#include "../../../../DirectX/Material/Material.h"
namespace MyDirectX
{
	class Material;
}

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
			//if (version >= 1) archive(material);
		}
		template<class Archive>
		void load(Archive & archive, std::uint32_t const version)
		{
			archive(cereal::make_nvp("Collision", cereal::base_class<Collision>(this)), CEREAL_NVP(radius));
			//if (version >= 1) archive(material);
		}

	public:
		CollisionSphere() {}
		CollisionSphere(Vector3 offset, float radius, bool isTrigger);
		CollisionSphere(std::string name, Vector3 offset, float radius, bool isTrigger);
		~CollisionSphere();

		void SetRadius(float radius);
		float GetRadius();

		std::weak_ptr<MyDirectX::Material> material;	// ÉRÉäÉWÉáÉìÇ…ëÆê´ÇéùÇΩÇπÇÈÇ∆Ç´Ç…égÇ§
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

CEREAL_CLASS_VERSION(FrameWork::CollisionSphere, 1)
CEREAL_REGISTER_TYPE(FrameWork::CollisionSphere)


#endif // !_COLLISIONSPHERE_H_

