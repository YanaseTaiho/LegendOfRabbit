#include "RayCast.h"
#include "../Collision.h"
#include "../Mesh/CollisionMesh.h"
#include "../Sphere/CollisionSphere.h"

using namespace FrameWork;

bool RayCast::JudgeAllCollision(Ray * ray, RayCastInfo * castInfo, std::weak_ptr<GameObject> myObject)
{
	bool hit = false;
	Vector3 rayCenter = Vector3::Lerp(ray->start, ray->end, 0.5f);
	float rayRadiusSq = (ray->end - ray->start).LengthSq() * 0.5f;

	for (int i = 0; i < (int)Layer::MAX; i++)
	{
		for (auto & collision : Collision::CollisionList(i))
		{
			Collision * col = collision.lock().get();

			if (!col->IsEnable()) continue;

			// 自身のオブジェクトか確認する
			if (!myObject.expired())
			{
				// 自身のコリジョンだった場合は判定をしない
				if (col->gameObject.lock() == myObject.lock()) continue;
			}

			// 球の判定をする
			if (col->GetType() == typeid(CollisionSphere))
			{
				RayCastInfo addInfo;
				if (JudgeSphere(*ray, col->worldMatrix.position(), col->scaleRadius, &addInfo))
				{
					if (!hit)
					{
						addInfo.collision = collision;
						*castInfo = addInfo;
						hit = true;
					}
					else if (castInfo->distance > addInfo.distance)
					{
						addInfo.collision = collision;
						*castInfo = addInfo;
					}
				}
				continue;
			}
			else
			{
				float rayDistSq = (rayCenter - col->worldMatrix.position()).LengthSq();
				float radiusSq = col->scaleRadius * col->scaleRadius + rayRadiusSq;
				if (rayDistSq > radiusSq)
					continue;
			}

			// 相手がメッシュコリジョンの場合
			if (col->GetType() == typeid(CollisionMesh))
			{
				// 判定をする相手の逆行列を掛ける
				Ray r = *ray;
				Matrix4 invTargetMat = col->worldMatrix.Inverse();
				r.start = invTargetMat * r.start;
				r.end = invTargetMat * r.end;

				CollisionMesh * colMesh = (CollisionMesh*)col;
				if (colMesh->meshInfo.expired()) continue;
				RayCastInfo addInfo;
				if (!JudgeMesh(&r, colMesh->meshInfo.lock().get(), &addInfo)) continue;

				// ワールド系に戻してから格納
				addInfo.point = col->worldMatrix * addInfo.point;
				addInfo.distance = (addInfo.point - ray->start).Length();

				if (!hit)
				{
					addInfo.collision = collision;
					*castInfo = addInfo;
					hit = true;

					// 法線はポジションを 0 の位置にしてから計算
					Matrix4 matrix = col->worldMatrix;
					matrix.matrix(0, 3) = 0.0f; matrix.matrix(1, 3) = 0.0f; matrix.matrix(2, 3) = 0.0f;
					castInfo->normal = matrix * castInfo->normal;
					castInfo->normal.Normalize();
				}
				else if (castInfo->distance > addInfo.distance)
				{
					addInfo.collision = collision;
					*castInfo = addInfo;

					// 法線はポジションを 0 の位置にしてから計算
					Matrix4 matrix = col->worldMatrix;
					matrix.matrix(0, 3) = 0.0f; matrix.matrix(1, 3) = 0.0f; matrix.matrix(2, 3) = 0.0f;
					castInfo->normal = matrix * castInfo->normal;
					castInfo->normal.Normalize();
				}

				continue;
			}
		}
	}

	return hit;
}

bool RayCast::JudgeMesh(Ray * ray, CollisionMeshInfo * collisionMesh, RayCastInfo * castInfo)
{
	RayCastInfo finalInfo;
	bool hit = false;
	for (auto & mesh : collisionMesh->faceInfoArray)
	{
		RayCastInfo addInfo;
		if (JudgeFace(ray, &mesh, &addInfo))
		{
			if (hit)
			{
				// 後に当たったポリゴンと距離を比較して前に当たったポリゴンより近かった場合は上書き
				if (finalInfo.distance > addInfo.distance)
					finalInfo = addInfo;
			}
			else
			{
				finalInfo = addInfo;
			}
			hit = true;
		}
	}
	
	if (hit)
	{
		*castInfo = finalInfo;
		return true;
	}

	return false;
}

bool RayCast::JudgeSphere(const Ray & ray, const Vector3 & pos, const float radius, RayCastInfo * castInfo)
{
	Vector3 RtoS = pos - ray.start;			// レイから球のベクトル
	Vector3 rayDir = ray.end - ray.start;	// レイの方向ベクトル

	float A = Vector3::Dot(rayDir, rayDir);
	float B = Vector3::Dot(rayDir, RtoS);
	float C = Vector3::Dot(RtoS, RtoS) - radius * radius;

	if (A == 0.0f)
		return false; // レイの長さが0

	float s = B * B - A * C;
	if (s < 0.0f)
		return false; // 衝突していない

	s = sqrtf(s);
	float a1 = (B - s) / A;	// 一つ目の衝突点までの長さ
	float a2 = (B + s) / A;	// 二つ目の衝突点までの長さ

	if (a1 < 0.0f || a2 < 0.0f)
		return false; // レイの反対で衝突

	if (a1 > 1.0f)
		return false;

	castInfo->point = ray.start + a1 * rayDir;
	castInfo->normal = castInfo->point - pos; castInfo->normal.Normalize();
	castInfo->distance = (castInfo->point - ray.start).Length();

	return true;
}

bool RayCast::JudgeInfinityCilinder(const Ray & ray, const Vector3 & start, const Vector3 & end, const float radius, RayCastInfo * castInfo)
{
	Vector3 rayDir = ray.end - ray.start;	// レイの方向ベクトル
	Vector3 p1 = start - ray.start;
	Vector3 p2 = end - ray.start;
	Vector3 p1_to_p2 = p2 - p1;

	// 各種内積値
	float Dvv = Vector3::Dot(rayDir, rayDir);// vx * vx + vy * vy + vz * vz;
	float Dsv = Vector3::Dot(rayDir, p1_to_p2);// sx * vx + sy * vy + sz * vz;
	float Dpv = Vector3::Dot(rayDir, p1);// px * vx + py * vy + pz * vz;
	float Dss = Vector3::Dot(p1_to_p2, p1_to_p2);// sx * sx + sy * sy + sz * sz;
	float Dps = Vector3::Dot(p1, p1_to_p2);// px * sx + py * sy + pz * sz;
	float Dpp = Vector3::Dot(p1, p1);// px * px + py * py + pz * pz;
	float rr = radius * radius;

	if (Dss == 0.0f)
		return false; // 円柱が定義されない

	float A = Dvv - Dsv * Dsv / Dss;
	float B = Dpv - Dps * Dsv / Dss;
	float C = Dpp - Dps * Dps / Dss - rr;

	if (A == 0.0f)
		return false;

	float s = B * B - A * C;
	if (s < 0.0f)
		return false; // レイが円柱と衝突していない
	s = sqrtf(s);

	float a1 = (B - s) / A;
	//float a2 = (B + s) / A;

	castInfo->point = ray.start + a1 * rayDir;
	castInfo->distance = (castInfo->point - ray.start).Length();

	Vector3 cDir = end - start; cDir.Normalize();
	Vector3 cpDir = castInfo->point - start;
	// 丸い部分
	Vector3 cpN = cpDir - Vector3::Dot(cDir, cpDir) * cDir;
	castInfo->normal = cpN;
	castInfo->normal.Normalize();
	return true;
}

bool RayCast::JudgeCilinder(const Ray & ray, const Vector3 & start, const Vector3 & end, const float radius, RayCastInfo * castInfo)
{
	Vector3 rayDir = ray.end - ray.start;	// レイの方向ベクトル
	Vector3 rayDirNor = rayDir.Normalized();	// レイの正規化方向ベクトル
	float rr = radius * radius;

	// シリンダーの端のスタート地点
	Vector3 cilinderNormal = start - end; cilinderNormal.Normalize();
	if (Vector3::Dot(-cilinderNormal, rayDirNor) > 0.0f)
	{
		Plane p;
		p.Set(start, cilinderNormal);
		float d1 = (p.a * ray.end.x) + (p.b * ray.end.y) + (p.c * ray.end.z);
		float d2 = (p.a * ray.start.x) + (p.b * ray.start.y) + (p.c * ray.start.z);
		//パラメトリック方程式の媒介変数” t "を解く。
		float t = -(d1 + p.d) / (d2 - d1);
		// t が０から１の間であるなら交差していることになる（この時点では、まだ無限遠平面との交差）
		if (t >= 0 && t <= 1.0)
		{
			//交点座標を得る　ｔが分かっていれば両端点から簡単に求まる
			Vector3 hitPoint;
			hitPoint = ray.start * t + ray.end * (1 - t);

			// ここでシリンダーの半径内なら衝突している
			Vector3 len = hitPoint - start;
			if (len.LengthSq() <= rr)
			{
				castInfo->point = hitPoint;
				castInfo->distance = (castInfo->point - ray.start).Length();
				castInfo->normal = cilinderNormal;
				return true;
			}
		}
	}
	// シリンダーの端のゴール地点
	cilinderNormal = -cilinderNormal;
	if (Vector3::Dot(-cilinderNormal, rayDirNor) > 0.0f)
	{
		Plane p;
		p.Set(end, cilinderNormal);
		float d1 = (p.a * ray.end.x) + (p.b * ray.end.y) + (p.c * ray.end.z);
		float d2 = (p.a * ray.start.x) + (p.b * ray.start.y) + (p.c * ray.start.z);
		//パラメトリック方程式の媒介変数” t "を解く。
		float t = -(d1 + p.d) / (d2 - d1);
		// t が０から１の間であるなら交差していることになる（この時点では、まだ無限遠平面との交差）
		if (t >= 0 && t <= 1.0)
		{
			//交点座標を得る　ｔが分かっていれば両端点から簡単に求まる
			Vector3 hitPoint;
			hitPoint = ray.start * t + ray.end * (1 - t);

			// ここでシリンダーの半径内なら衝突している
			Vector3 len = hitPoint - end;
			if (len.LengthSq() <= rr)
			{
				castInfo->point = hitPoint;
				castInfo->distance = (castInfo->point - ray.start).Length();
				castInfo->normal = cilinderNormal;
				return true;
			}
		}
	}

	// シリンダーの側面を調べる
	Vector3 p1 = start - ray.start;
	Vector3 p2 = end - ray.start;
	Vector3 p1_to_p2 = p2 - p1;

	// 各種内積値
	float Dvv = Vector3::Dot(rayDir, rayDir);
	float Dsv = Vector3::Dot(rayDir, p1_to_p2);
	float Dpv = Vector3::Dot(rayDir, p1);
	float Dss = Vector3::Dot(p1_to_p2, p1_to_p2);
	float Dps = Vector3::Dot(p1, p1_to_p2);
	float Dpp = Vector3::Dot(p1, p1);

	if (Dss == 0.0f)
		return false; // 円柱が定義されない

	float A = Dvv - Dsv * Dsv / Dss;
	float B = Dpv - Dps * Dsv / Dss;
	float C = Dpp - Dps * Dps / Dss - rr;

	if (A == 0.0f)
		return false;

	float s = B * B - A * C;
	if (s < 0.0f)
		return false; // レイが円柱と衝突していない
	s = sqrtf(s);

	float a1 = (B - s) / A;
//	float a2 = (B + s) / A;

	castInfo->point = ray.start + a1 * rayDir;

	Vector3 cDir = end - start;
	Vector3 cpDir = castInfo->point - start;
	float cilinderLength = cDir.Length(); cDir.Normalize();
	float cilinderRange = Vector3::Dot(cDir, cpDir);

	if (cilinderRange < 0.0f || cilinderRange > cilinderLength)
		return false;

	castInfo->distance = (castInfo->point - ray.start).Length();
	// 丸い部分
	Vector3 cpN = cpDir - Vector3::Dot(cDir, cpDir) * cDir;
	castInfo->normal = cpN;
	castInfo->normal.Normalize();

	return true;
}

bool RayCast::JudgeCapsule(const Ray & ray, const Vector3 & start, const Vector3 & end, const float radius, RayCastInfo * castInfo)
{
	// Q1の検査
	if (
		JudgeSphere(ray, start, radius, castInfo) == true &&
		CheckDot(end, start, castInfo->point) <= 0.0f
		) 
	{
		return true; // Q1は球面P1上にある
	}
	else if (
		JudgeSphere(ray, end, radius, castInfo) == true &&
		CheckDot(start, end, castInfo->point) <= 0.0f
		) 
	{
		return true; // Q1は球面P2上にある
	}
	else if (
		JudgeInfinityCilinder(ray, start, end, radius, castInfo) == true &&
		CheckDot(start, end, castInfo->point) > 0.0f &&
		CheckDot(end, start, castInfo->point) > 0.0f
		) 
	{
		return true; // Q1は円柱面にある
	}

	// レイは衝突していない
	return false;
}

bool RayCast::JudgeFace(Ray * ray, CollisionFaceInfo * collisionMesh, RayCastInfo * castInfo)
{
	Plane p;
	p.Set(*collisionMesh);
	float d1 = (p.a * ray->end.x) + (p.b * ray->end.y) + (p.c * ray->end.z);
	float d2 = (p.a * ray->start.x) + (p.b * ray->start.y) + (p.c * ray->start.z);
	//パラメトリック方程式の媒介変数” t "を解く。
	float t = -(d1 + p.d) / (d2 - d1);
	// t が０から１の間であるなら交差していることになる（この時点では、まだ無限遠平面との交差）
	if (t >= 0 && t <= 1.0)
	{
		//交点座標を得る　ｔが分かっていれば両端点から簡単に求まる
		Vector3 hitPoint;
		hitPoint = ray->start * t + ray->end * (1 - t);

		//交点が三角形の内か外かを判定　（ここで内部となれば、完全な交差となる）
		if (IsInside(&hitPoint, &collisionMesh->point[0], &collisionMesh->point[1], &collisionMesh->point[2], &collisionMesh->normal) == true)
		{
			castInfo->point = hitPoint;
			castInfo->normal = collisionMesh->normal;
			castInfo->distance = (hitPoint - ray->start).Length();
			return true;
		}
	}

	return false;
}

bool RayCast::IsInside(Vector3 * point, Vector3 * p1, Vector3 * p2, Vector3 * p3, Vector3 * normal)
{
	//辺ベクトル
	Vector3 vAB, vBC, vCA;
	vAB = *p2 - *p1;
	vBC = *p3 - *p2;
	vCA = *p1 - *p3;
	//辺ベクトルと「頂点から交点へ向かうベクトル」との、それぞれの外積用
	Vector3 vCrossAB, vCrossBC, vCrossCA;
	//「外積結果のベクトル」と平面法線ベクトルとの、それぞれの内積用
	float fAB, fBC, fCA;

	// 各頂点から交点Iに向かうベクトルをvVとする
	Vector3 vV;
	// 辺ABベクトル（頂点Bベクトル-頂点Aベクトル)と、頂点Aから交点Iへ向かうベクトル、の外積を求める
	vV = *point - *p1;
	vCrossAB = Vector3::Cross(vAB, vV);
	// 辺BCベクトル（頂点Cベクトル-頂点Bベクトル)と、頂点Bから交点Iへ向かうベクトル、の外積を求める
	vV = *point - *p2;
	vCrossBC = Vector3::Cross(vBC, vV);
	// 辺CAベクトル（頂点Aベクトル-頂点Cベクトル)と、頂点Cから交点Iへ向かうベクトル、の外積を求める
	vV = *point - *p3;
	vCrossCA = Vector3::Cross(vCA, vV);
	// それぞれの、外積ベクトルとの内積を計算する
	fAB = Vector3::Dot(*normal, vCrossAB);
	fBC = Vector3::Dot(*normal, vCrossBC);
	fCA = Vector3::Dot(*normal, vCrossCA);

	// ３つの内積結果のうち、一つでもマイナス符号のものがあれば、交点は外にある。
	if (fAB >= 0 && fBC >= 0 && fCA >= 0)
	{
		// 交点は、面の内にある
		return true;
	}
	// 交点は面の外にある
	return false;
}

float RayCast::CheckDot(const Vector3 & p1, const Vector3 & p2, const Vector3 & p3)
{
	return (p1.x - p2.x) * (p3.x - p2.x) + (p1.y - p2.y) * (p3.y - p2.y) + (p1.z - p2.z) * (p3.z - p2.z);
}
