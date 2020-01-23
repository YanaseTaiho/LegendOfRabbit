#ifndef _COLLISIONCAPSULE_H_
#define _COLLISIONCAPSULE_H_

#include "../Collision.h"

namespace FrameWork
{
	class CollisionCapsule : public Collision
	{
	private:
		friend cereal::access;
		template<class Archive>
		void save(Archive & archive) const
		{
			archive(cereal::make_nvp("Collision", cereal::base_class<Collision>(this)),
				CEREAL_NVP(radius), CEREAL_NVP(localStart), CEREAL_NVP(localEnd), CEREAL_NVP(worldStart), CEREAL_NVP(worldEnd));
		}
		template<class Archive>
		void load(Archive & archive)
		{
			archive(cereal::make_nvp("Collision", cereal::base_class<Collision>(this)),
				CEREAL_NVP(radius), CEREAL_NVP(localStart), CEREAL_NVP(localEnd), CEREAL_NVP(worldStart), CEREAL_NVP(worldEnd));
		}

	public:
		CollisionCapsule() {}
		CollisionCapsule(Vector3 offset, float radius, Vector3 start, Vector3 end, bool isTrigger);
		CollisionCapsule(std::string name ,Vector3 offset, float radius, Vector3 start, Vector3 end, bool isTrigger);
		~CollisionCapsule();

		void SetCapsule(float radius, Vector3 start, Vector3 end);

	private:
		float radius;
		Vector3 localStart;
		Vector3 localEnd;
		Vector3 worldStart;
		Vector3 worldEnd;

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

CEREAL_REGISTER_TYPE(FrameWork::CollisionCapsule)


#endif //!_COLLISIONCAPSULE_H_
