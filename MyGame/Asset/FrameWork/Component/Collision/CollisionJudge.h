#ifndef _COLLISIONJUDGE_H_
#define _COLLISIONJUDGE_H_

#include "Collision.h"

namespace FrameWork
{
	class CollisionJudge
	{
	public:
		static bool Sphere_VS_Sphere(CollisionSphere * a, CollisionSphere * b);
		static bool Sphere_VS_Capsule(CollisionSphere * a, CollisionCapsule * b);
		static bool Sphere_VS_Box(CollisionSphere * a, CollisionBox * b);
		static bool Sphere_VS_Mesh(CollisionSphere * a, CollisionMesh * b);

		static bool Capsule_VS_Capsule(CollisionCapsule * a, CollisionCapsule * b);

		static bool Box_VS_Box(CollisionBox * a, CollisionBox * b);

		// 点と線分の最短ベクトルを返す
		static Vector3 Point_VS_Line(const Vector3 & p, const Vector3 & lp1, const Vector3 & lp2);

	private:

		CollisionJudge() {}
		~CollisionJudge() {}
	};
}

#endif // !_COLLISIONJUDGE_H_

