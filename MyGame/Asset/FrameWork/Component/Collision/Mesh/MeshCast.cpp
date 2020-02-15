#include "MeshCast.h"
#include "../Mesh/CollisionMesh.h"
#include "../Sphere/CollisionSphere.h"
#include "../CollisionJudge.h"
#include "../../../../DirectX/Renderer/MeshRenderer.h"

using namespace FrameWork;
using namespace MyDirectX;

bool MeshCast::JudgeAllCollision(const std::vector<MeshPoints> & meshPoints, std::function<void(MeshCastInfo & hitInfo)> callBack, const std::list<std::weak_ptr<Collision>> & myCollisions)
{
	bool isHit = false;

	Vector3 total;
	for (auto p : meshPoints)
	{
		total += p.point[0] + p.point[1] + p.point[2] + p.point[3];
	}
	Vector3 center = total / ((float)meshPoints.size() * 4.0f);
	float radiusSq = 0.0f;
	for (auto p : meshPoints)
	{
		float len = (center - p.point[0]).LengthSq();
		if (len > radiusSq) radiusSq = len;
		len = (center - p.point[1]).LengthSq();
		if (len > radiusSq) radiusSq = len;
		len = (center - p.point[2]).LengthSq();
		if (len > radiusSq) radiusSq = len;
		len = (center - p.point[3]).LengthSq();
		if (len > radiusSq) radiusSq = len;
	}

	for (int i = 0; i < (int)Layer::MAX; i++)
	{
		for (auto & collision : Collision::CollisionList(i))
		{
			Collision * col = collision.lock().get();

			if (!col->IsEnable()) continue;

			// 自身のオブジェクトか確認する
			bool myHit = false;
			for (auto my : myCollisions)
			{
				// 自身のコリジョンだった場合は判定をしない
				if (my.lock() == collision.lock()) {
					myHit = true;
					break;
				}
			}
			if (myHit) continue;


			// 球の判定をする
			float rayDistSq = (center - col->worldMatrix.position()).LengthSq();
			float RadiusSq = col->scaleRadius * col->scaleRadius + radiusSq;
			if (rayDistSq > RadiusSq)
				continue;

			
			if (col->GetType() == typeid(CollisionSphere))
			{
				CollisionSphere * sphere = (CollisionSphere*)col;

				for (auto points : meshPoints)
				{
					Vector3 p[3];
					p[0] = points.point[0]; p[1] = points.point[1]; p[2] = points.point[2];
					if (Sphere_VS_Mesh(sphere, p))
					{
						isHit = true;
						if (callBack)
						{
							MeshCastInfo hitInfo;
							hitInfo.collision = collision;
							hitInfo.point = (p[0] + p[1] + p[2]) / 3;
							hitInfo.normal = hitInfo.point - collision.lock()->worldMatrix.position();
							hitInfo.normal.Normalize();
							callBack(hitInfo);
						}
						break;
					}
					p[0] = points.point[1]; p[1] = points.point[2]; p[2] = points.point[3];
					if (Sphere_VS_Mesh(sphere, p))
					{
						isHit = true;
						if (callBack)
						{
							MeshCastInfo hitInfo;
							hitInfo.collision = collision;
							hitInfo.point = (p[0] + p[1] + p[2]) / 3;
							hitInfo.normal = hitInfo.point - collision.lock()->worldMatrix.position();
							hitInfo.normal.Normalize();
							callBack(hitInfo);
						}
						break;
					}
				}
				continue;
			}

			// 相手がメッシュコリジョンの場合
			if (col->GetType() == typeid(CollisionMesh))
			{
				CollisionMesh * colMesh = (CollisionMesh*)col;

				// 判定をする相手の逆行列を掛ける
				Vector3 v[4];
				Matrix4 invTargetMat = col->worldMatrix.Inverse();

				for (auto points : meshPoints)
				{
					v[0] = invTargetMat * points.point[0];
					v[1] = invTargetMat * points.point[1];
					v[2] = invTargetMat * points.point[2];
					v[3] = invTargetMat * points.point[3];

					bool hit = false;

					for (auto face : colMesh->meshInfo.lock()->faceInfoArray)
					{
						Vector3 p[3];
						p[0] = v[0]; p[1] = v[1]; p[2] = v[2];
						if (intersectTriangleTriangle(p, face.point))
						{
							isHit = true;
							if (callBack)
							{
								MeshCastInfo hitInfo;
								hitInfo.collision = collision;
								hitInfo.point = (points.point[0] + points.point[1] + points.point[2]) / 3;
								hitInfo.normal = face.normal;
								// マテリアル情報取得
								auto renderer = hitInfo.collision.lock()->gameObject.lock()->GetComponent<MeshRenderer>();
								if (!renderer.expired())
								{
									hitInfo.material = renderer.lock()->GetMaterial(face.materialIndex);
								}

								callBack(hitInfo);
							}
							hit = true;
							break;
						}
						p[0] = v[1]; p[1] = v[2]; p[2] = v[3];
						if (intersectTriangleTriangle(p, face.point))
						{
							isHit = true;
							if (callBack)
							{
								MeshCastInfo hitInfo;
								hitInfo.collision = collision;
								hitInfo.point = (points.point[1] + points.point[2] + points.point[3]) / 3;
								hitInfo.normal = face.normal;

								// マテリアル情報取得
								auto renderer = hitInfo.collision.lock()->gameObject.lock()->GetComponent<MeshRenderer>();
								if (!renderer.expired())
								{
									hitInfo.material = renderer.lock()->GetMaterial(face.materialIndex);
								}

								callBack(hitInfo);
							}
							hit = true;
							break;
						}
					}

					if (hit) break;
				}
				continue;
			}
		}
	}

	return isHit;
}

bool MeshCast::Sphere_VS_Mesh(const CollisionSphere * a, const Vector3 point[3])
{
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
			Vector3 h;

			h = CollisionJudge::Point_VS_Line(SC, point[i], point[(i + 1) % 3]);
			if (a->scaleRadius * a->scaleRadius > h.LengthSq())
			{
				return true;
			}
		}
	}

	return false;
}
