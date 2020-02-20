#include "RayCast.h"
#include "../Collision.h"
#include "../Mesh/CollisionMesh.h"
#include "../Sphere/CollisionSphere.h"
#include "../../../GameObject/GameObject.h"
#include "../../../../DirectX/Renderer/MeshRenderer.h"

using namespace FrameWork;
using namespace MyDirectX;

bool RayCast::JudgeAllCollision(Ray * ray, RayCastInfo * castInfo, std::weak_ptr<GameObject> myObject, int layerMask, bool isFaceHit)
{
	bool hit = false;
	Vector3 rayCenter = Vector3::Lerp(ray->start, ray->end, 0.5f);
	float rayRadiusSq = (ray->end - ray->start).LengthSq() * 0.5f;

	for (int i = 0; i < (int)Layer::MAX; i++)
	{
		// ���C���[�}�X�N���画�肷�邩�m�F
		if (layerMask >= 0 && (layerMask & (1 << i))) continue;

		for (auto & collision : Collision::CollisionList(i))
		{
			Collision * col = collision.lock().get();

			if (!col->IsEnable()) continue;
			if (col->isTrigger) continue;	// �g���K�[���I���̏ꍇ�͖���

			// ���g�̃I�u�W�F�N�g���m�F����
			if (!myObject.expired())
			{
				// ���g�̃R���W�����������ꍇ�͔�������Ȃ�
				if (col->gameObject.lock() == myObject.lock()) continue;
				//// ���g�̎q�̃R���W�����ł���������
				//if (myObject.lock()->transform.lock()->IsChild(col->transform)) continue;
			}

			// ���̔��������
			if (col->GetType() == typeid(CollisionSphere))
			{
				RayCastInfo addInfo;
				if (JudgeSphere(*ray, col->worldMatrix.position(), col->scaleRadius, &addInfo))
				{
					if (!hit)
					{
						addInfo.collision = collision;
						// �}�e���A�����擾�p�̃����_�[�擾
						addInfo.material = ((CollisionSphere*)col)->material;
						*castInfo = addInfo;
						hit = true;
					}
					else if (castInfo->distance > addInfo.distance)
					{
						addInfo.collision = collision;
						// �}�e���A�����擾�p�̃����_�[�擾
						addInfo.material = ((CollisionSphere*)col)->material;
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

			// ���肪���b�V���R���W�����̏ꍇ
			if (col->GetType() == typeid(CollisionMesh))
			{
				// ��������鑊��̋t�s����|����
				Ray r = *ray;
				Matrix4 invTargetMat = col->worldMatrix.Inverse();
				r.start = invTargetMat * r.start;
				r.end = invTargetMat * r.end;

				CollisionMesh * colMesh = (CollisionMesh*)col;
				if (colMesh->meshInfo.expired()) continue;
				RayCastInfo addInfo;
				// ���C�ƃ��b�V���̏Փ˔���
				if (!JudgeMesh(&r, colMesh, &addInfo)) continue;

				if (isFaceHit)
				{
					// �@�����������C�̋t�����Ȃ�Փ˂��Ă��Ȃ�����Ƃ���
					Vector3 rayDir = (r.start - r.end).Normalized();
					if (Vector3::Dot(addInfo.normal, rayDir) < 0) continue;
				}

				// ���[���h�n�ɖ߂��Ă���i�[
				addInfo.point = col->worldMatrix * addInfo.point;
				addInfo.distance = (addInfo.point - ray->start).Length();

				if (!hit)
				{
					addInfo.collision = collision;
					*castInfo = addInfo;
					hit = true;

					// �@���̓|�W�V������ 0 �̈ʒu�ɂ��Ă���v�Z
					Matrix4 matrix = col->worldMatrix;
					matrix.matrix(0, 3) = 0.0f; matrix.matrix(1, 3) = 0.0f; matrix.matrix(2, 3) = 0.0f;
					castInfo->normal = matrix * castInfo->normal;
					castInfo->normal.Normalize();
				}
				else if (castInfo->distance > addInfo.distance)
				{
					addInfo.collision = collision;
					*castInfo = addInfo;

					// �@���̓|�W�V������ 0 �̈ʒu�ɂ��Ă���v�Z
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

bool RayCast::JudgeAllCollision(Ray * ray, RayCastInfo * castInfo, const std::list<std::weak_ptr<Collision>> & myCollisions, int layerMask, bool isFaceHit)
{
	bool hit = false;
	Vector3 rayCenter = Vector3::Lerp(ray->start, ray->end, 0.5f);
	float rayRadiusSq = (ray->end - ray->start).LengthSq() * 0.5f;

	for (int i = 0; i < (int)Layer::MAX; i++)
	{
		// ���C���[�}�X�N���画�肷�邩�m�F
		if (layerMask >= 0 && (layerMask & (1 << i))) continue;

		for (auto & collision : Collision::CollisionList(i))
		{
			Collision * col = collision.lock().get();

			if (!col->IsEnable()) continue;
			if (col->isTrigger) continue;	// �g���K�[���I���̏ꍇ�͖���

			// ���g�̃I�u�W�F�N�g���m�F����
			bool myHit = false;
			for (auto my : myCollisions)
			{
				// ���g�̃R���W�����������ꍇ�͔�������Ȃ�
				if (my.lock() == collision.lock()) {
					myHit = true;
					break;
				}
			}
			if(myHit) continue;

			// ���̔��������
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

			// ���肪���b�V���R���W�����̏ꍇ
			if (col->GetType() == typeid(CollisionMesh))
			{
				// ��������鑊��̋t�s����|����
				Ray r = *ray;
				Matrix4 invTargetMat = col->worldMatrix.Inverse();
				r.start = invTargetMat * r.start;
				r.end = invTargetMat * r.end;

				CollisionMesh * colMesh = (CollisionMesh*)col;
				if (colMesh->meshInfo.expired()) continue;
				RayCastInfo addInfo;
				// ���C�ƃ��b�V���̏Փ˔���
				if (!JudgeMesh(&r, colMesh, &addInfo)) continue;

				if (isFaceHit)
				{
					// �@�����������C�̋t�����Ȃ�Փ˂��Ă��Ȃ�����Ƃ���
					Vector3 rayDir = (r.start - r.end).Normalized();
					if (Vector3::Dot(addInfo.normal, rayDir) < 0) continue;
				}

				// ���[���h�n�ɖ߂��Ă���i�[
				addInfo.point = col->worldMatrix * addInfo.point;
				addInfo.distance = (addInfo.point - ray->start).Length();

				if (!hit)
				{
					addInfo.collision = collision;
					*castInfo = addInfo;
					hit = true;

					// �@���̓|�W�V������ 0 �̈ʒu�ɂ��Ă���v�Z
					Matrix4 matrix = col->worldMatrix;
					matrix.matrix(0, 3) = 0.0f; matrix.matrix(1, 3) = 0.0f; matrix.matrix(2, 3) = 0.0f;
					castInfo->normal = matrix * castInfo->normal;
					castInfo->normal.Normalize();
				}
				else if (castInfo->distance > addInfo.distance)
				{
					addInfo.collision = collision;
					*castInfo = addInfo;

					// �@���̓|�W�V������ 0 �̈ʒu�ɂ��Ă���v�Z
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

bool RayCast::JudgeCollision(Ray * ray, RayCastInfo * castInfo, std::weak_ptr<Collision> other)
{
	auto col = other.lock().get();

	// ���̔��������
	if (col->GetType() == typeid(CollisionSphere))
	{
		if (JudgeSphere(*ray, col->worldMatrix.position(), col->scaleRadius, castInfo))
		{
			castInfo->collision = other;
			return true;
		}
	}
	// ���肪���b�V���R���W�����̏ꍇ
	else if (col->GetType() == typeid(CollisionMesh))
	{
		// ��������鑊��̋t�s����|����
		Ray r = *ray;
		Matrix4 invTargetMat = col->worldMatrix.Inverse();
		r.start = invTargetMat * r.start;
		r.end = invTargetMat * r.end;

		CollisionMesh * colMesh = (CollisionMesh*)col;
		if (colMesh->meshInfo.expired()) return false;
		// ���C�ƃ��b�V���̏Փ˔���
		if (!JudgeMesh(&r, colMesh, castInfo)) return false;

		//if (isFaceHit)
		//{
		//	// �@�����������C�̋t�����Ȃ�Փ˂��Ă��Ȃ�����Ƃ���
		//	Vector3 rayDir = (r.start - r.end).Normalized();
		//	if (Vector3::Dot(castInfo->normal, rayDir) < 0) return false;
		//}

		// ���[���h�n�ɖ߂��Ă���i�[
		castInfo->point = col->worldMatrix * castInfo->point;
		castInfo->distance = (castInfo->point - ray->start).Length();

		castInfo->collision = other;

		// �@���̓|�W�V������ 0 �̈ʒu�ɂ��Ă���v�Z
		Matrix4 matrix = col->worldMatrix;
		matrix.matrix(0, 3) = 0.0f; matrix.matrix(1, 3) = 0.0f; matrix.matrix(2, 3) = 0.0f;
		castInfo->normal = matrix * castInfo->normal;
		castInfo->normal.Normalize();

		return true;
	}

	return false;
}

bool RayCast::JudgeMesh(Ray * ray, CollisionMesh * collisionMesh, RayCastInfo * castInfo)
{
	RayCastInfo finalInfo;
	bool hit = false;
	auto meshInfo = collisionMesh->meshInfo.lock();
	// �}�e���A�����擾�p�̃����_�[�擾
	auto renderer = collisionMesh->gameObject.lock()->GetComponent<MeshRenderer>();

	for (auto & mesh : meshInfo->faceInfoArray)
	{
		RayCastInfo addInfo;
		if (JudgeFace(ray, &mesh, &addInfo))
		{
			if (hit)
			{
				// ��ɓ��������|���S���Ƌ������r���đO�ɓ��������|���S�����߂������ꍇ�͏㏑��
				if (finalInfo.distance > addInfo.distance)
				{
					finalInfo = addInfo;
					// �}�e���A�����擾
					if (!renderer.expired())
					{
						finalInfo.material = renderer.lock()->GetMaterial(mesh.materialIndex);
					}
				}
			}
			else
			{
				finalInfo = addInfo;
				// �}�e���A�����擾
				if (!renderer.expired())
				{
					finalInfo.material = renderer.lock()->GetMaterial(mesh.materialIndex);
				}
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
	Vector3 RtoS = pos - ray.start;			// ���C���狅�̃x�N�g��
	Vector3 rayDir = ray.end - ray.start;	// ���C�̕����x�N�g��

	float A = Vector3::Dot(rayDir, rayDir);
	float B = Vector3::Dot(rayDir, RtoS);
	float C = Vector3::Dot(RtoS, RtoS) - radius * radius;

	if (A == 0.0f)
		return false; // ���C�̒�����0

	float s = B * B - A * C;
	if (s < 0.0f)
		return false; // �Փ˂��Ă��Ȃ�

	s = sqrtf(s);
	float a1 = (B - s) / A;	// ��ڂ̏Փ˓_�܂ł̒���
	float a2 = (B + s) / A;	// ��ڂ̏Փ˓_�܂ł̒���

	if (a1 < 0.0f || a2 < 0.0f)
		return false; // ���C�̔��΂ŏՓ�

	if (a1 > 1.0f)
		return false;

	castInfo->point = ray.start + a1 * rayDir;
	castInfo->normal = castInfo->point - pos; castInfo->normal.Normalize();
	castInfo->distance = (castInfo->point - ray.start).Length();

	return true;
}

bool RayCast::JudgeInfinityCilinder(const Ray & ray, const Vector3 & start, const Vector3 & end, const float radius, RayCastInfo * castInfo)
{
	Vector3 rayDir = ray.end - ray.start;	// ���C�̕����x�N�g��
	Vector3 p1 = start - ray.start;
	Vector3 p2 = end - ray.start;
	Vector3 p1_to_p2 = p2 - p1;

	// �e����ϒl
	float Dvv = Vector3::Dot(rayDir, rayDir);// vx * vx + vy * vy + vz * vz;
	float Dsv = Vector3::Dot(rayDir, p1_to_p2);// sx * vx + sy * vy + sz * vz;
	float Dpv = Vector3::Dot(rayDir, p1);// px * vx + py * vy + pz * vz;
	float Dss = Vector3::Dot(p1_to_p2, p1_to_p2);// sx * sx + sy * sy + sz * sz;
	float Dps = Vector3::Dot(p1, p1_to_p2);// px * sx + py * sy + pz * sz;
	float Dpp = Vector3::Dot(p1, p1);// px * px + py * py + pz * pz;
	float rr = radius * radius;

	if (Dss == 0.0f)
		return false; // �~������`����Ȃ�

	float A = Dvv - Dsv * Dsv / Dss;
	float B = Dpv - Dps * Dsv / Dss;
	float C = Dpp - Dps * Dps / Dss - rr;

	if (A == 0.0f)
		return false;

	float s = B * B - A * C;
	if (s < 0.0f)
		return false; // ���C���~���ƏՓ˂��Ă��Ȃ�
	s = sqrtf(s);

	float a1 = (B - s) / A;
	//float a2 = (B + s) / A;

	castInfo->point = ray.start + a1 * rayDir;
	castInfo->distance = (castInfo->point - ray.start).Length();

	Vector3 cDir = end - start; cDir.Normalize();
	Vector3 cpDir = castInfo->point - start;
	// �ۂ�����
	Vector3 cpN = cpDir - Vector3::Dot(cDir, cpDir) * cDir;
	castInfo->normal = cpN;
	castInfo->normal.Normalize();
	return true;
}

bool RayCast::JudgeCilinder(const Ray & ray, const Vector3 & start, const Vector3 & end, const float radius, RayCastInfo * castInfo)
{
	Vector3 rayDir = ray.end - ray.start;	// ���C�̕����x�N�g��
	Vector3 rayDirNor = rayDir.Normalized();	// ���C�̐��K�������x�N�g��
	float rr = radius * radius;

	// �V�����_�[�̒[�̃X�^�[�g�n�_
	Vector3 cilinderNormal = start - end; cilinderNormal.Normalize();
	if (Vector3::Dot(-cilinderNormal, rayDirNor) > 0.0f)
	{
		Plane p;
		p.Set(start, cilinderNormal);
		float d1 = (p.a * ray.end.x) + (p.b * ray.end.y) + (p.c * ray.end.z);
		float d2 = (p.a * ray.start.x) + (p.b * ray.start.y) + (p.c * ray.start.z);
		//�p�����g���b�N�������̔}��ϐ��h t "�������B
		float t = -(d1 + p.d) / (d2 - d1);
		// t ���O����P�̊Ԃł���Ȃ�������Ă��邱�ƂɂȂ�i���̎��_�ł́A�܂����������ʂƂ̌����j
		if (t >= 0 && t <= 1.0)
		{
			//��_���W�𓾂�@�����������Ă���Η��[�_����ȒP�ɋ��܂�
			Vector3 hitPoint;
			hitPoint = ray.start * t + ray.end * (1 - t);

			// �����ŃV�����_�[�̔��a���Ȃ�Փ˂��Ă���
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
	// �V�����_�[�̒[�̃S�[���n�_
	cilinderNormal = -cilinderNormal;
	if (Vector3::Dot(-cilinderNormal, rayDirNor) > 0.0f)
	{
		Plane p;
		p.Set(end, cilinderNormal);
		float d1 = (p.a * ray.end.x) + (p.b * ray.end.y) + (p.c * ray.end.z);
		float d2 = (p.a * ray.start.x) + (p.b * ray.start.y) + (p.c * ray.start.z);
		//�p�����g���b�N�������̔}��ϐ��h t "�������B
		float t = -(d1 + p.d) / (d2 - d1);
		// t ���O����P�̊Ԃł���Ȃ�������Ă��邱�ƂɂȂ�i���̎��_�ł́A�܂����������ʂƂ̌����j
		if (t >= 0 && t <= 1.0)
		{
			//��_���W�𓾂�@�����������Ă���Η��[�_����ȒP�ɋ��܂�
			Vector3 hitPoint;
			hitPoint = ray.start * t + ray.end * (1 - t);

			// �����ŃV�����_�[�̔��a���Ȃ�Փ˂��Ă���
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

	// �V�����_�[�̑��ʂ𒲂ׂ�
	Vector3 p1 = start - ray.start;
	Vector3 p2 = end - ray.start;
	Vector3 p1_to_p2 = p2 - p1;

	// �e����ϒl
	float Dvv = Vector3::Dot(rayDir, rayDir);
	float Dsv = Vector3::Dot(rayDir, p1_to_p2);
	float Dpv = Vector3::Dot(rayDir, p1);
	float Dss = Vector3::Dot(p1_to_p2, p1_to_p2);
	float Dps = Vector3::Dot(p1, p1_to_p2);
	float Dpp = Vector3::Dot(p1, p1);

	if (Dss == 0.0f)
		return false; // �~������`����Ȃ�

	float A = Dvv - Dsv * Dsv / Dss;
	float B = Dpv - Dps * Dsv / Dss;
	float C = Dpp - Dps * Dps / Dss - rr;

	if (A == 0.0f)
		return false;

	float s = B * B - A * C;
	if (s < 0.0f)
		return false; // ���C���~���ƏՓ˂��Ă��Ȃ�
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
	// �ۂ�����
	Vector3 cpN = cpDir - Vector3::Dot(cDir, cpDir) * cDir;
	castInfo->normal = cpN;
	castInfo->normal.Normalize();

	return true;
}

bool RayCast::JudgeCapsule(const Ray & ray, const Vector3 & start, const Vector3 & end, const float radius, RayCastInfo * castInfo)
{
	// Q1�̌���
	if (
		JudgeSphere(ray, start, radius, castInfo) == true &&
		CheckDot(end, start, castInfo->point) <= 0.0f
		) 
	{
		return true; // Q1�͋���P1��ɂ���
	}
	else if (
		JudgeSphere(ray, end, radius, castInfo) == true &&
		CheckDot(start, end, castInfo->point) <= 0.0f
		) 
	{
		return true; // Q1�͋���P2��ɂ���
	}
	else if (
		JudgeInfinityCilinder(ray, start, end, radius, castInfo) == true &&
		CheckDot(start, end, castInfo->point) > 0.0f &&
		CheckDot(end, start, castInfo->point) > 0.0f
		) 
	{
		return true; // Q1�͉~���ʂɂ���
	}

	// ���C�͏Փ˂��Ă��Ȃ�
	return false;
}

bool RayCast::JudgeFace(Ray * ray, CollisionFaceInfo * collisionMesh, RayCastInfo * castInfo)
{
	Plane p;
	p.Set(*collisionMesh);
	float d1 = (p.a * ray->end.x) + (p.b * ray->end.y) + (p.c * ray->end.z);
	float d2 = (p.a * ray->start.x) + (p.b * ray->start.y) + (p.c * ray->start.z);
	//�p�����g���b�N�������̔}��ϐ��h t "�������B
	float t = -(d1 + p.d) / (d2 - d1);
	// t ���O����P�̊Ԃł���Ȃ�������Ă��邱�ƂɂȂ�i���̎��_�ł́A�܂����������ʂƂ̌����j
	if (t >= 0 && t <= 1.0)
	{
		//��_���W�𓾂�@�����������Ă���Η��[�_����ȒP�ɋ��܂�
		Vector3 hitPoint;
		hitPoint = ray->start * t + ray->end * (1 - t);

		//��_���O�p�`�̓����O���𔻒�@�i�����œ����ƂȂ�΁A���S�Ȍ����ƂȂ�j
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
	//�Ӄx�N�g��
	Vector3 vAB, vBC, vCA;
	vAB = *p2 - *p1;
	vBC = *p3 - *p2;
	vCA = *p1 - *p3;
	//�Ӄx�N�g���Ɓu���_�����_�֌������x�N�g���v�Ƃ́A���ꂼ��̊O�ϗp
	Vector3 vCrossAB, vCrossBC, vCrossCA;
	//�u�O�ό��ʂ̃x�N�g���v�ƕ��ʖ@���x�N�g���Ƃ́A���ꂼ��̓��ϗp
	float fAB, fBC, fCA;

	// �e���_�����_I�Ɍ������x�N�g����vV�Ƃ���
	Vector3 vV;
	// ��AB�x�N�g���i���_B�x�N�g��-���_A�x�N�g��)�ƁA���_A�����_I�֌������x�N�g���A�̊O�ς����߂�
	vV = *point - *p1;
	vCrossAB = Vector3::Cross(vAB, vV);
	// ��BC�x�N�g���i���_C�x�N�g��-���_B�x�N�g��)�ƁA���_B�����_I�֌������x�N�g���A�̊O�ς����߂�
	vV = *point - *p2;
	vCrossBC = Vector3::Cross(vBC, vV);
	// ��CA�x�N�g���i���_A�x�N�g��-���_C�x�N�g��)�ƁA���_C�����_I�֌������x�N�g���A�̊O�ς����߂�
	vV = *point - *p3;
	vCrossCA = Vector3::Cross(vCA, vV);
	// ���ꂼ��́A�O�σx�N�g���Ƃ̓��ς��v�Z����
	fAB = Vector3::Dot(*normal, vCrossAB);
	fBC = Vector3::Dot(*normal, vCrossBC);
	fCA = Vector3::Dot(*normal, vCrossCA);

	// �R�̓��ό��ʂ̂����A��ł��}�C�i�X�����̂��̂�����΁A��_�͊O�ɂ���B
	if (fAB >= 0 && fBC >= 0 && fCA >= 0)
	{
		// ��_�́A�ʂ̓��ɂ���
		return true;
	}
	// ��_�͖ʂ̊O�ɂ���
	return false;
}

float RayCast::CheckDot(const Vector3 & p1, const Vector3 & p2, const Vector3 & p3)
{
	return (p1.x - p2.x) * (p3.x - p2.x) + (p1.y - p2.y) * (p3.y - p2.y) + (p1.z - p2.z) * (p3.z - p2.z);
}
