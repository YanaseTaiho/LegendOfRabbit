#ifndef _COLLISIONMESHINFO_H_
#define _COLLISIONMESHINFO_H_

#include "../../../Vector.h"
#include <vector>

namespace FrameWork
{
	struct CollisionFaceInfo
	{	
		Vector3 point[3];
		Vector3 normal;

	private:
		friend cereal::access;
		template <typename Archive>
		void serialize(Archive & archive)
		{
			archive(CEREAL_NVP(point), CEREAL_NVP(normal));
		}
	};

	struct Plane
	{
		void Set(const Vector3 * p1, const Vector3 * p2, const Vector3 * p3)
		{
			Vector3 vecAB = *p2 - *p1;
			Vector3 vecBC = *p3 - *p2;
			vecAB.Normalize();
			vecBC.Normalize();
			Vector3 normal = Vector3::Cross(vecAB, vecBC);
			a = normal.x;
			b = normal.y;
			c = normal.z;
			d = -(a * p1->x + b * p1->y + c * p1->z);

		}

		void Set(const Vector3 & point, const Vector3 & normal)
		{
			a = normal.x;
			b = normal.y;
			c = normal.z;
			d = -(a * point.x + b * point.y + c * point.z);
		}

		void Set(const CollisionFaceInfo & info)
		{
			// ñ@ê¸
			a = info.normal.x;
			b = info.normal.y;
			c = info.normal.z;
			// d ÇåvéZ
			d = -(a * info.point[0].x + b * info.point[0].y + c * info.point[0].z);
		}
		float a, b, c, d;
	};

	struct CollisionMeshInfo
	{
		std::string name;
		std::vector<CollisionFaceInfo> faceInfoArray;
		float scaleRadius;
	
	private:
		friend cereal::access;
		template <typename Archive>
		void serialize(Archive & archive)
		{
			archive(CEREAL_NVP(faceInfoArray), CEREAL_NVP(scaleRadius));
		}
	};
}

#endif // !_COLLISIONMESHINFO_H_
