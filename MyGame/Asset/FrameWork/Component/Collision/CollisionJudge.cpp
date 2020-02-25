#include "CollisionJudge.h"
#include "../Rigidbody/Rigidbody.h"

#include "Sphere/CollisionSphere.h"
#include "Box/CollisionBox.h"
#include "Mesh/CollisionMesh.h"

using namespace FrameWork;

bool CollisionJudge::Sphere_VS_Sphere(CollisionSphere * a, CollisionSphere * b)
{
	Vector3 distance = a->worldMatrix.position() - b->worldMatrix.position();
	bool isHit = false;
	
	float lengthSq = distance.LengthSq();
	float radiusLength = a->radius + b->radius;

	isHit = lengthSq <= radiusLength * radiusLength;

	// 当たってない場合、どちらかがトリガーならリターン
	if (!isHit || a->isTrigger || b->isTrigger) return isHit;

	// 押し出し処理
	float length = sqrtf(lengthSq);
	float pushLength = radiusLength - length;	// 押し出す距離
	Vector3 pushVec = Vector3::Normalize(distance);	// 押し出す方向

	bool isRigidA = !a->rigidbody.expired();
	bool isRigidB = !b->rigidbody.expired();

	// 片方のリジッドボディがセットされていたら
	if (isRigidA && !isRigidB)
	{
		Transform *rbA = a->rigidbody.lock()->transform.lock().get();
		Vector3 position = rbA->GetWorldPosition();
		rbA->SetWorldPosition(position - (pushVec * pushLength));
	}
	else if (!isRigidA && isRigidB)
	{
		Transform *rbB = b->rigidbody.lock()->transform.lock().get();
		Vector3 position = rbB->GetWorldPosition();
		rbB->SetWorldPosition(position - (pushVec * pushLength));
	}
	// 両方のリジッドボディがセットされていたら
	else if (isRigidA && isRigidB)
	{
		Vector3 move = (pushVec * pushLength) * 0.5f;

		Transform *rbA = a->rigidbody.lock()->transform.lock().get();
		Vector3 positionA = rbA->GetWorldPosition();
		rbA->SetWorldPosition(positionA + move);

		Transform *rbB = b->rigidbody.lock()->transform.lock().get();
		Vector3 positionB = rbB->GetWorldPosition();
		rbB->SetWorldPosition(positionB - move);
	}

	return isHit;
}

bool CollisionJudge::Sphere_VS_Capsule(CollisionSphere * a, CollisionCapsule * b)
{

	return false;
}

bool CollisionJudge::Sphere_VS_Box(CollisionSphere * a, CollisionBox * b)
{
	return false;
}

bool CollisionJudge::Sphere_VS_Mesh(CollisionSphere * a, CollisionMesh * b)
{
	if (b->meshInfo.expired()) return false;
	if (a->isTrigger) return false;	// 球がトリガーなら重いので当たり判定をしない

	bool isHit = false;

	Vector3 SC = a->worldMatrix.position();	// 球の中心座標
	Vector3 SCR = SC + Vector3(a->radius, 0.0f, 0.0f);
	Matrix4 invMat = b->worldMatrix.Inverse();
	SC = invMat * SC;	// 球体の位置をメッシュコリジョンのワールド空間の逆行列を掛ける
	SCR = invMat * SCR;
	SCR = SC - SCR;
	float aRadius = SCR.Length();	// 半径も同様に変換

	float per = a->radius / aRadius;	// 変換前に戻す割合( 押し出し処理で使う )

	auto & faceArray = b->meshInfo.lock()->faceInfoArray;

	// 三角ポリゴンと球の判定
	for (auto & face : faceArray)
	{
		Vector3 CP = SC - face.point[0];	// 球の中心からポリゴンの一点とのベクトル
		float CP_Dot = Vector3::Dot(CP, face.normal);	// 球の中心と平面との距離を算出
		float def = aRadius - fabsf(CP_Dot);	// 球の半径と平面との距離が衝突しているか判定( 後で押し出し処理に使う )

		// 負の値ならこのポリゴンとは当たっていない
		if (def < 0) continue;

		// 平面との判定
		{
			bool hit = true;

			Vector3 point = SC - (face.normal * CP_Dot);	// 球の中心から平面に下した座標

			Vector3 vAP = point - face.point[0];
			Vector3 vBP = point - face.point[1];
			Vector3 vCP = point - face.point[2];

			Vector3 nP;

			// 一つでも条件が一致したらポリゴン上にない
			nP = Vector3::Cross(vAP, vBP);
			if (Vector3::Dot(nP, face.normal) < 0) hit = false;
			nP = Vector3::Cross(vBP, vCP);
			if (Vector3::Dot(nP, face.normal) < 0) hit = false;
			nP = Vector3::Cross(vCP, vAP);
			if (Vector3::Dot(nP, face.normal) < 0) hit = false;

			if (hit)
			{
				isHit = true;

				// どちらかがトリガーオンなら当たったことだけ知らせる
				if (a->isTrigger || b->isTrigger) break;

				// 押し出し処理
				bool isRigidA = !a->rigidbody.expired();
				bool isRigidB = !b->rigidbody.expired();
				// 片方のリジッドボディがセットされていたら
				if (isRigidA && !isRigidB)
				{
					Transform *rbA = a->rigidbody.lock()->transform.lock().get();
					Vector3 position = rbA->GetWorldPosition();
					float defW = def * per;
					// 法線をワールド空間に変換
					Matrix4 noPosMat = b->worldMatrix;
					noPosMat.matrix(0, 3) = 0.0f; noPosMat.matrix(1, 3) = 0.0f; noPosMat.matrix(2, 3) = 0.0f;
					Vector3 normal = noPosMat * face.normal;
					normal.Normalize();

					if (Vector3::Dot(Vector3::up(), normal) <= a->pushDotUp)
					{

						//float gravityAngle = Mathf::RadToDeg(Mathf::ACosf(Vector3::Dot(Vector3::up(), normal)));
						//// 一定の角度以下なら滑らない
						//if (gravityAngle <= 45.1f)
						//{
						//	point = b->worldMatrix * point;	// ヒット座標をワールド座標に変換
						//	Vector3 pushVec = a->worldMatrix.position() - point;
						//	float pushForce = pushVec.Length() * def;

						//	if (CP_Dot > 0)	// ポリゴンの表側にある場合
						//		position.y += pushForce;
						//	else  // ポリゴンの裏側にある場合
						//		position.y += pushForce + (a->radius * def);
						//	
					//		if(a->rigidbody.lock()->velocity.y < 0.0f)
						//		a->rigidbody.lock()->velocity.y = 0.0f;
						//}
						//else
						{
							if (CP_Dot > 0)	// ポリゴンの表側にある場合
							{
								SC += (normal * def);
								position += (normal * defW);
							}
							else  // ポリゴンの裏側にある場合
							{
								SC += normal * (def + a->radius);
								position += normal * (defW + a->radius);
							}
							// 押し出す方向の逆に働いている力を打ち消す
							float force = Vector3::Dot(normal, a->rigidbody.lock()->velocity);
							if (force < 0.0f)
								a->rigidbody.lock()->velocity -= normal * force;
						}

						rbA->SetWorldPosition(position);
						a->Update();
					}
				}
				continue;
			}
		}
		// 辺との判定
		{
			bool hit = false;
			// まだ当たってない場合次に各辺と衝突しているか判定
			for (int i = 0; i < 3; i++)
			{
				Vector3 h,hNor;
				float def;

				h = Point_VS_Line(SC, face.point[i], face.point[(i + 1) % 3]);
				def = aRadius - h.Length();
				if (def > 0)
				{
					hNor = h.Normalized();
					// 辺と球の中心の垂直ベクトルが平面の法線と角度が一定以上なら当たっていないと判定する
					float deg = Mathf::RadToDeg(Mathf::ACosf(Vector3::Dot(hNor, face.normal)));
					if (deg >= 45.0f) continue;

					hit = true;
					// どちらかがトリガーオンなら当たったことだけ知らせる
					if (a->isTrigger || b->isTrigger) break;

					// 押し出し処理
					bool isRigidA = !a->rigidbody.expired();
					bool isRigidB = !b->rigidbody.expired();
					// 片方のリジッドボディがセットされていたら
					if (isRigidA && !isRigidB)
					{
						Transform *rbA = a->rigidbody.lock()->transform.lock().get();
						Vector3 position = rbA->GetWorldPosition();
						float defW = def * per;
						// 法線をワールド空間に変換
						Matrix4 noPosMat = b->worldMatrix;
						noPosMat.matrix(0, 3) = 0.0f; noPosMat.matrix(1, 3) = 0.0f; noPosMat.matrix(2, 3) = 0.0f;
						Vector3 normal = noPosMat * face.normal;
						normal.Normalize();

						if (Vector3::Dot(Vector3::up(), normal) <= a->pushDotUp)
						{
							//float gravityAngle = Mathf::RadToDeg(Mathf::ACosf(Vector3::Dot(Vector3::up(), normal)));
							//// 一定の角度以下なら滑らない
							//if (gravityAngle <= 45.1f)
							//{
							//	float pushForce = def;

							//	if (CP_Dot > 0)	// ポリゴンの表側にある場合
							//		position.y += pushForce;
							//	else  // ポリゴンの裏側にある場合
							//		position.y += pushForce + (a->radius * def);

							//	if (a->rigidbody.lock()->velocity.y < 0.0f)
							//		a->rigidbody.lock()->velocity.y = 0.0f;
							//}
							//else
							{
								if (CP_Dot > 0)	// ポリゴンの表側にある場合
								{
									SC += (normal * def);
									position += (normal * defW);
								}
								else  // ポリゴンの裏側にある場合
								{
									SC += normal * (def + a->radius);
									position += normal * (defW + a->radius);
								}

								// 押し出す方向の逆に働いている力を打ち消す
								float force = Vector3::Dot(normal, a->rigidbody.lock()->velocity);
								if (force < 0.0f)
									a->rigidbody.lock()->velocity -= normal * force;
							}

							rbA->SetWorldPosition(position);
							a->Update();
						}
						break;
					}
				}
			}

			isHit = hit;
		}
		// 頂点の当たり判定( なくても一応動作する )
		{
			//bool hit = false;

			//Vector3 h, hNor;
			//float def;
			//float hLen;

			//for (int i = 0; i < 3; i++)
			//{
			//	h = SC - face.point[i];
			//	
			//	hLen = h.Length();
			//	def = aRadius - hLen;
			//	if (def > 0.0f)
			//	{
			//		hit = true;
			//		break;
			//	}
			//}
			//
			//
			//if (hit)
			//{
			//	hNor = h.Normalized();

			//	// 辺と球の中心の垂直ベクトルが平面の法線と角度が一定以上なら当たっていないと判定する
			//	float deg = Mathf::RadToDeg(Mathf::ACosf(Vector3::Dot(hNor, face.normal)));
			//	if (deg >= 45.0f) continue;

			//	isHit = true;
			//	// どちらかがトリガーオンなら当たったことだけ知らせる
			//	if (a->isTrigger || b->isTrigger) break;

			//	// 押し出し処理
			//	bool isRigidA = !a->rigidbody.expired();
			//	bool isRigidB = !b->rigidbody.expired();
			//	// 片方のリジッドボディがセットされていたら
			//	if (isRigidA && !isRigidB)
			//	{
			//		Transform *rbA = a->rigidbody.lock()->transform.lock().get();
			//		Vector3 position = rbA->GetWorldPosition();
			//		def *= per;
			//		// 法線をワールド空間に変換
			//		Matrix4 noPosMat = b->worldMatrix;
			//		noPosMat.matrix(0, 3) = 0.0f; noPosMat.matrix(1, 3) = 0.0f; noPosMat.matrix(2, 3) = 0.0f;
			//		Vector3 normal = noPosMat * face.normal;
			//		normal.Normalize();

			//		float gravityAngle = Mathf::RadToDeg(Mathf::ACosf(Vector3::Dot(Vector3::up(), normal)));
			//		// 一定の角度以下なら滑らない
			//		if (gravityAngle <= 45.1f)
			//		{
			//			hLen *= per;
			//			float pushForce = hLen * def;

			//			if (CP_Dot > 0)	// ポリゴンの表側にある場合
			//				position.y += pushForce;
			//			else  // ポリゴンの裏側にある場合
			//				position.y += pushForce + (a->radius * def);

			//			if (a->rigidbody.lock()->velocity.y < 0.0f)
			//				a->rigidbody.lock()->velocity.y = 0.0f;
			//		}
			//		else
			//		{
			//			if (CP_Dot > 0)	// ポリゴンの表側にある場合
			//				position = position + (normal * def);
			//			else  // ポリゴンの裏側にある場合
			//				position = position + (normal * (def + a->radius));

			//			// 押し出す方向の逆に働いている力を打ち消す
			//			float force = Vector3::Dot(normal, a->rigidbody.lock()->velocity);
			//			a->rigidbody.lock()->velocity -= normal * force;
			//		}

			//		rbA->SetWorldPosition(position);
			//		a->Update();
			//	}
			//}
		}
		
	}

	return isHit;
}

bool CollisionJudge::Capsule_VS_Capsule(CollisionCapsule * a, CollisionCapsule * b)
{
	return false;
}

bool CollisionJudge::Box_VS_Box(CollisionBox * a, CollisionBox * b)
{
	return false;
}

Vector3 CollisionJudge::Point_VS_Line(const Vector3 & p, const Vector3 & lp1, const Vector3 & lp2)
{
	Vector3 v1 = lp2 - lp1;
	Vector3 v2 = p - lp1;
	Vector3 h;
	float t = Vector3::Dot(v1.Normalized(), v2) / v1.Length();	// 点から線分に垂直に下す際の割合

	if (t < 0.0f)
		h = v2;
	else if (t > 1.0f)
		h = p - lp2;
	else
		h = -(v1 * t) + v2;

	return h;
}
