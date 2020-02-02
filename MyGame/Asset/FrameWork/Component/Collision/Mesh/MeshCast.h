#ifndef _MESH_CAST_H_
#define _MESH_CAST_H_

#include "../../../Vector.h"

namespace FrameWork
{
	class CollisionSphere;

	class MeshCast
	{
	public:

		static bool JudgeAllCollision(const Vector3 point[4]);

		static bool Sphere_VS_Mesh(const CollisionSphere * a, const Vector3 point[4]);
		

		static int pointInTriangle2D(const Vector2 &p, const Vector2 &a, const Vector2 &b, const Vector2 &c)
		{
			float pab = Vector2::Cross(p - a, b - a);
			float pbc = Vector2::Cross(p - b, c - b);
			if (pab * pbc < 0)
				return 0;
			float pca = Vector2::Cross(p - c, a - c);
			if (pab * pca < 0)
				return 0;
			return 1;
		}

		static float Signed2DTriArea(const Vector2 &a, const Vector2 &b, const Vector2 &c)
		{
			return (a.x - c.x) * (b.y - c.y) - (a.y - c.y) * (b.x - c.x);
		}

		static int test2DSegmentSegment(const Vector2 &a, const Vector2 &b, const Vector2 &c, const Vector2 &d, float &t, Vector2 &p) {
			float a1 = Signed2DTriArea(a, b, d);
			float a2 = Signed2DTriArea(a, b, c);
			if (a1 * a2 < 0.0f) {
				float a3 = Signed2DTriArea(c, d, a);
				float a4 = a3 + a2 - a1;
				if (a3 * a4 < 0.0f) {
					t = a3 / (a3 - a4);
					p = a + t * (b - a);
					return 1;
				}
			}
			return 0;
		}

		///////////////////////////////////////////
		// 三角ポリゴンと三角ポリゴンの衝突判定
		//
		//  t1[3] : 三角ポリゴン1の頂点座標
		//  t2[3] : 三角ポリゴン2の頂点座標
		//  戻り値: 衝突ならtrue

		static bool intersectTriangleTriangle(const Vector3 t1[3], const Vector3 t2[3])
		{
			// t1を含む平面とt0の3頂点の距離の符号が同じならば衝突していない
			Vector3 N1, N2;
			N1 = Vector3::Cross((t1[1] - t1[0]), (t1[2] - t1[0]));
			N1.Normalize();
			N2 = Vector3::Cross((t2[1] - t2[0]), (t2[2] - t2[0]));
			N2.Normalize();
			float d1 = Vector3::Dot((-N1), t1[0]);
			float d2 = Vector3::Dot((-N2), t2[0]);
			float dist1[3], dist2[3];

			unsigned sign1 = 0, sign2 = 0;
			for (unsigned i = 0; i < 3; i++) 
			{
				dist1[i] = Vector3::Dot(N2, t1[i]) + d2;
				if (dist1[i] > 0)
					sign1 |= (1 << i);
				dist2[i] = Vector3::Dot(N1, t2[i]) + d1;
				if (dist2[i] > 0)
					sign2 |= (1 << i);
			}
			if (sign1 == 0 || sign1 == 7 || sign2 == 0 || sign2 == 7) 
			{
				// 全dist1 == 0の場合は同一平面衝突の可能性がある
				if (fabsf(dist1[0]) >= 0.00001f || fabsf(dist1[1]) >= 0.00001f || fabsf(dist1[2]) >= 0.00001f)
					return false;
				// 三角ポリゴンの頂点を軸並行平面へ投影
				Vector2 t1_2D[3], t2_2D[3];
				if (fabsf(N1.x) >= fabsf(N1.y) && fabsf(N1.x) >= fabsf(N1.z)) {
					// YZ平面
					for (unsigned i = 0; i < 3; i++) {
						t1_2D[i].x = t1[i].y; t1_2D[i].y = t1[i].z;
						t2_2D[i].x = t2[i].y; t2_2D[i].y = t2[i].z;
					}
				}
				else if (fabsf(N1.y) >= fabsf(N1.z)) {
					// XZ平面
					for (unsigned i = 0; i < 3; i++) {
						t1_2D[i].x = t1[i].x; t1_2D[i].y = t1[i].z;
						t2_2D[i].x = t2[i].x; t2_2D[i].y = t2[i].z;
					}
				}
				else {
					// XY平面
					for (unsigned i = 0; i < 3; i++) {
						t1_2D[i].x = t1[i].x; t1_2D[i].y = t1[i].y;
						t2_2D[i].x = t2[i].x; t2_2D[i].y = t2[i].y;
					}
				}
				// 線分間衝突
				float t;
				Vector2 p;
				for (unsigned i = 0; i < 3; i++) {
					for (unsigned j = 0; j < 3; j++) {
						if (test2DSegmentSegment(t1_2D[i], t1_2D[(i + 1) % 3], t2_2D[j], t2_2D[(j + 1) % 3], t, p))
							return true;
					}
				}

				// 点含有
				for (unsigned i = 0; i < 3; i++) {
					if (pointInTriangle2D(t1_2D[i], t2_2D[0], t2_2D[1], t2_2D[2]))
						return true;
				}
				for (unsigned i = 0; i < 3; i++) {
					if (pointInTriangle2D(t2_2D[i], t1_2D[0], t1_2D[1], t1_2D[2]))
						return true;
				}

				// 残念・・・
				return false;
			}

			/////////////////////////////////
			// 同一平面には無い
			////

			// 頂点の片側を合わせるためのハッシュテーブル
			static struct Hash { int i1, i0, i2; } hash[7] = {
				{0, 0, 0},
				{0, 1, 2}, // idx0
				{1, 0, 2}, // idx1
				{2, 0, 1}, // idx2
				{2, 0, 1}, // idx2
				{1, 0, 2}, // idx1
				{0, 1, 2}, // idx0
			};

			// 2三角形の共通線Lで交わっているか？
			Vector3 D;
			D = Vector3::Cross(N1,N2);
			D.Normalize();
			{
				float p1[3] = {
					Vector3::Dot(D, t1[hash[sign1].i0]),
					Vector3::Dot(D, t1[hash[sign1].i1]),
					Vector3::Dot(D, t1[hash[sign1].i2]),
				};
				float p2[3] = {
					Vector3::Dot(D, t2[hash[sign2].i0]),
					Vector3::Dot(D, t2[hash[sign2].i1]),
					Vector3::Dot(D, t2[hash[sign2].i2]),
				};
				float d1[3] = {
					dist1[hash[sign1].i0],
					dist1[hash[sign1].i1],
					dist1[hash[sign1].i2],
				};
				float d2[3] = {
					dist2[hash[sign2].i0],
					dist2[hash[sign2].i1],
					dist2[hash[sign2].i2],
				};

				float t1_1 = p1[0] + (p1[1] - p1[0]) * d1[0] / (d1[0] - d1[1]);
				float t1_2 = p1[2] + (p1[1] - p1[2]) * d1[2] / (d1[2] - d1[1]);
				float t2_1 = p2[0] + (p2[1] - p2[0]) * d2[0] / (d2[0] - d2[1]);
				float t2_2 = p2[2] + (p2[1] - p2[2]) * d2[2] / (d2[2] - d2[1]);

				// t1_1～t1_2の間にある？
				if (t1_1 < t1_2) {
					if (t2_1 < t2_2) {
						if (t2_2 < t1_1 || t1_2 < t2_1)
							return false;
					}
					else {
						if (t2_1 < t1_1 || t1_2 < t2_2)
							return false;
					}
				}
				else {
					if (t2_1 < t2_2) {
						if (t2_2 < t1_2 || t1_1 < t2_1)
							return false;
					}
					else {
						if (t2_1 < t1_2 || t1_1 < t2_2)
							return false;
					}
				}
			}
			return true;
		}
	};
}

#endif // !_MESH_CAST_H_
