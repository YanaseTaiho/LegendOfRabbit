#ifndef _COLLISIONBOX_H_
#define _COLLISIONBOX_H_

#include "../Collision.h"

namespace FrameWork
{
	class CollisionBox : public Collision
	{
	private:
		friend cereal::access;
		template<class Archive>
		void save(Archive & archive) const
		{
			archive(cereal::make_nvp("Collision", cereal::base_class<Collision>(this)));
		}
		template<class Archive>
		void load(Archive & archive)
		{
			archive(cereal::make_nvp("Collision", cereal::base_class<Collision>(this)));
		}

	public:
		CollisionBox();
		~CollisionBox();

	private:
		void Update() override;
		void Draw() override;

		bool CollisionJudge(Collision * other) override;

		bool vsCollisionSphere(CollisionSphere * other) override;
		bool vsCollisionCapsule(CollisionCapsule * other) override;
		bool vsCollisionBox(CollisionBox * other) override;
		bool vsCollisionMesh(CollisionMesh * other) override;
	};
}

CEREAL_REGISTER_TYPE(FrameWork::CollisionBox)


#endif // !_COLLISIONBOX_H_

