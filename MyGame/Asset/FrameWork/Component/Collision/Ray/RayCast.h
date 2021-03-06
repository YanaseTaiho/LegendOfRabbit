#ifndef _RAYCAST_H_
#define _RAYCAST_H_

#include "Ray.h"
#include "../Mesh/CollisionMeshInfo.h"
#include <memory>

namespace MyDirectX
{
	class Material;
}

namespace FrameWork
{
	class Collision;
	class GameObject;
	class CollisionMesh;

	struct RayCastInfo
	{
		Vector3 point;
		Vector3 normal;
		float distance;
		std::weak_ptr<Collision> collision;
		std::weak_ptr<MyDirectX::Material> material;
	};

	class RayCast
	{
	public:
		static bool JudgeAllCollision(Ray * ray, RayCastInfo * castInfo, std::weak_ptr<GameObject> myObject = std::weak_ptr<GameObject>(), int layerMask = -1, bool isFaceHit = false);
		static bool JudgeAllCollision(Ray * ray, RayCastInfo * castInfo, const std::list<std::weak_ptr<Collision>> & myCollisions, int layerMask = -1, bool isFaceHit = false);
		static bool JudgeCollision(Ray * ray, RayCastInfo * castInfo, std::weak_ptr<Collision> other);
		static bool JudgeMesh(Ray * ray, CollisionMesh * collisionMesh, RayCastInfo * castInfo);
		static bool JudgeSphere(const Ray & ray, const Vector3 & pos, const float radius, RayCastInfo * castInfo);
		static bool JudgeInfinityCilinder(const Ray & ray, const Vector3 & start, const Vector3 & end, const float radius, RayCastInfo * castInfo);
		static bool JudgeCilinder(const Ray & ray, const Vector3 & start, const Vector3 & end, const float radius, RayCastInfo * castInfo);
		static bool JudgeCapsule(const Ray & ray, const Vector3 & start, const Vector3 & end, const float radius, RayCastInfo * castInfo);

	private:
		static bool JudgeFace(Ray * ray, CollisionFaceInfo * collisionMesh, RayCastInfo * castInfo);
		static bool IsInside(Vector3 * point, Vector3 * p1, Vector3 * p2, Vector3 * p3, Vector3 * normal);
		static float CheckDot(const Vector3 & p1, const Vector3 & p2, const Vector3 & p3); // ��P1P2P3�̓��ς��Z�o
	};
}

#endif // !_RAYCAST_H_
