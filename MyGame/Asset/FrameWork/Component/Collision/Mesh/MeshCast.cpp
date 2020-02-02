#include "MeshCast.h"
#include "../Mesh/CollisionMesh.h"
#include "../Sphere/CollisionSphere.h"
#include "../CollisionJudge.h"

using namespace FrameWork;

bool MeshCast::JudgeAllCollision(const Vector3 point[4])
{
	bool isHit = false;

	Vector3 center = (point[0] + point[1] + point[2] + point[3]) / 4;
	float radiusSq = 0.0f;
	float len = (center - point[0]).LengthSq();
	if (len > radiusSq) radiusSq = len;
	len = (center - point[1]).LengthSq();
	if (len > radiusSq) radiusSq = len;
	len = (center - point[2]).LengthSq();
	if (len > radiusSq) radiusSq = len;
	len = (center - point[3]).LengthSq();
	if (len > radiusSq) radiusSq = len;

	for (int i = 0; i < (int)Layer::MAX; i++)
	{
		for (auto & collision : Collision::CollisionList(i))
		{
			Collision * col = collision.lock().get();

			if (!col->IsEnable()) continue;

			// 球の判定をする
			if (col->GetType() == typeid(CollisionSphere))
			{
				CollisionSphere * sphere = (CollisionSphere*)col;
				Vector3 p[3];
				p[0] = point[0]; p[1] = point[1]; p[2] = point[2];
				if(Sphere_VS_Mesh(sphere, p))
					isHit = true;
				p[0] = point[1]; p[1] = point[2]; p[2] = point[3];
				if (Sphere_VS_Mesh(sphere, p))
					isHit = true;

				continue;
			}
			else
			{
				float rayDistSq = (center - col->worldMatrix.position()).LengthSq();
				float RadiusSq = col->scaleRadius * col->scaleRadius + radiusSq;
				if (rayDistSq > RadiusSq)
					continue;
			}

			// 相手がメッシュコリジョンの場合
			if (col->GetType() == typeid(CollisionMesh))
			{
				CollisionMesh * colMesh = (CollisionMesh*)col;

				// 判定をする相手の逆行列を掛ける
				Vector3 v[4];
				Matrix4 invTargetMat = col->worldMatrix.Inverse();
				v[0] = invTargetMat * point[0];
				v[1] = invTargetMat * point[1];
				v[2] = invTargetMat * point[2];
				v[3] = invTargetMat * point[3];

				for(auto face : colMesh->meshInfo.lock()->faceInfoArray)
				{
					Vector3 p[3];
					p[0] = v[0]; p[1] = v[1]; p[2] = v[2];
					if (intersectTriangleTriangle(p, face.point))
					{
						isHit = true;
					}
					p[0] = v[1]; p[1] = v[2]; p[2] = v[3];
					if (intersectTriangleTriangle(p, face.point))
					{
						isHit = true;
					}
				}
				continue;
			}
		}
	}

	return isHit;
}

bool MeshCast::Sphere_VS_Mesh(const CollisionSphere * a, const Vector3 point[3])
{
	bool isHit = false;

	Vector3 SC = a->worldMatrix.position();	// 球の中心座標
	Vector3 vecAB = point[1] - point[0];
	Vector3 vecBC = point[2] - point[1];
	Vector3 normal = Vector3::Cross(vecAB, vecBC);
	normal.Normalize();

	// 三角ポリゴンと球の判定

	Vector3 CP = SC - point[0];	// 球の中心からポリゴンの一点とのベクトル
	float CP_Dot = Vector3::Dot(CP, normal);	// 球の中心と平面との距離を算出
	float def = a->scaleRadius - fabsf(CP_Dot);	// 球の半径と平面との距離が衝突しているか判定( 後で押し出し処理に使う )

	// 負の値ならこのポリゴンとは当たっていない
	if (def < 0) return false;

	// 平面との判定
	{
		bool hit = true;

		Vector3 p = SC - (normal * CP_Dot);	// 球の中心から平面に下した座標

		Vector3 vAP = p - point[0];
		Vector3 vBP = p - point[1];
		Vector3 vCP = p - point[2];

		Vector3 nP;

		// 一つでも条件が一致したらポリゴン上にない
		nP = Vector3::Cross(vAP, vBP);
		if (Vector3::Dot(nP, normal) < 0) hit = false;
		nP = Vector3::Cross(vBP, vCP);
		if (Vector3::Dot(nP, normal) < 0) hit = false;
		nP = Vector3::Cross(vCP, vAP);
		if (Vector3::Dot(nP, normal) < 0) hit = false;

		if (hit)
		{
			return true;
		}
	}
	// 辺との判定
	{
		// まだ当たってない場合次に各辺と衝突しているか判定
		for (int i = 0; i < 3; i++)
		{
			Vector3 h, hNor;
			float def;

			h = CollisionJudge::Point_VS_Line(SC, point[i], point[(i + 1) % 3]);
			def = a->scaleRadius - h.Length();
			if (def > 0)
			{
				//hNor = h.Normalized();
				//// 辺と球の中心の垂直ベクトルが平面の法線と角度が一定以上なら当たっていないと判定する
				//float deg = Mathf::RadToDeg(Mathf::ACosf(Vector3::Dot(hNor, normal)));
				//if (deg >= 45.0f) continue;

				return true;
			}
		}
	}

	return false;
}
