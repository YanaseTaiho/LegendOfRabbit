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

	// �������ĂȂ��ꍇ�A�ǂ��炩���g���K�[�Ȃ烊�^�[��
	if (!isHit || a->isTrigger || b->isTrigger) return isHit;

	// �����o������
	float length = sqrtf(lengthSq);
	float pushLength = radiusLength - length;	// �����o������
	Vector3 pushVec = Vector3::Normalize(distance);	// �����o������

	bool isRigidA = !a->rigidbody.expired();
	bool isRigidB = !b->rigidbody.expired();

	// �Е��̃��W�b�h�{�f�B���Z�b�g����Ă�����
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
	// �����̃��W�b�h�{�f�B���Z�b�g����Ă�����
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

	bool isHit = false;

	Vector3 SC = a->worldMatrix.position();	// ���̒��S���W
	Vector3 SCR = SC + Vector3(a->radius, 0.0f, 0.0f);
	Matrix4 invMat = b->worldMatrix.Inverse();
	SC = invMat * SC;	// ���̂̈ʒu�����b�V���R���W�����̃��[���h��Ԃ̋t�s����|����
	SCR = invMat * SCR;
	SCR = SC - SCR;
	float aRadius = SCR.Length();	// ���a�����l�ɕϊ�

	float per = a->radius / aRadius;	// �ϊ��O�ɖ߂�����( �����o�������Ŏg�� )

	auto & faceArray = b->meshInfo.lock()->faceInfoArray;

	// �O�p�|���S���Ƌ��̔���
	for (auto & face : faceArray)
	{
		Vector3 CP = SC - face.point[0];	// ���̒��S����|���S���̈�_�Ƃ̃x�N�g��
		float CP_Dot = Vector3::Dot(CP, face.normal);	// ���̒��S�ƕ��ʂƂ̋������Z�o
		float def = aRadius - fabsf(CP_Dot);	// ���̔��a�ƕ��ʂƂ̋������Փ˂��Ă��邩����( ��ŉ����o�������Ɏg�� )

		// ���̒l�Ȃ炱�̃|���S���Ƃ͓������Ă��Ȃ�
		if (def < 0) continue;

		// ���ʂƂ̔���
		{
			bool hit = true;

			Vector3 point = SC - (face.normal * CP_Dot);	// ���̒��S���畽�ʂɉ��������W

			Vector3 vAP = point - face.point[0];
			Vector3 vBP = point - face.point[1];
			Vector3 vCP = point - face.point[2];

			Vector3 nP;

			// ��ł���������v������|���S����ɂȂ�
			nP = Vector3::Cross(vAP, vBP);
			if (Vector3::Dot(nP, face.normal) < 0) hit = false;
			nP = Vector3::Cross(vBP, vCP);
			if (Vector3::Dot(nP, face.normal) < 0) hit = false;
			nP = Vector3::Cross(vCP, vAP);
			if (Vector3::Dot(nP, face.normal) < 0) hit = false;

			if (hit)
			{
				isHit = true;

				// �ǂ��炩���g���K�[�I���Ȃ瓖���������Ƃ����m�点��
				if (a->isTrigger || b->isTrigger) break;

				// �����o������
				bool isRigidA = !a->rigidbody.expired();
				bool isRigidB = !b->rigidbody.expired();
				// �Е��̃��W�b�h�{�f�B���Z�b�g����Ă�����
				if (isRigidA && !isRigidB)
				{
					Transform *rbA = a->rigidbody.lock()->transform.lock().get();
					Vector3 position = rbA->GetWorldPosition();
					float defW = def * per;
					// �@�������[���h��Ԃɕϊ�
					Matrix4 noPosMat = b->worldMatrix;
					noPosMat.matrix(0, 3) = 0.0f; noPosMat.matrix(1, 3) = 0.0f; noPosMat.matrix(2, 3) = 0.0f;
					Vector3 normal = noPosMat * face.normal;
					normal.Normalize();

					//float gravityAngle = Mathf::RadToDeg(Mathf::ACosf(Vector3::Dot(Vector3::up(), normal)));
					//// ���̊p�x�ȉ��Ȃ犊��Ȃ�
					//if (gravityAngle <= 45.1f)
					//{
					//	point = b->worldMatrix * point;	// �q�b�g���W�����[���h���W�ɕϊ�
					//	Vector3 pushVec = a->worldMatrix.position() - point;
					//	float pushForce = pushVec.Length() * def;

					//	if (CP_Dot > 0)	// �|���S���̕\���ɂ���ꍇ
					//		position.y += pushForce;
					//	else  // �|���S���̗����ɂ���ꍇ
					//		position.y += pushForce + (a->radius * def);
					//	
 				//		if(a->rigidbody.lock()->velocity.y < 0.0f)
					//		a->rigidbody.lock()->velocity.y = 0.0f;
					//}
					//else
					{
						if (CP_Dot > 0)	// �|���S���̕\���ɂ���ꍇ
						{
							SC += (normal * def);
							position += (normal * defW);
						}
						else  // �|���S���̗����ɂ���ꍇ
						{
							SC += normal * (def + a->radius);
							position += normal * (defW + a->radius);
						}
						// �����o�������̋t�ɓ����Ă���͂�ł�����
						float force = Vector3::Dot(normal, a->rigidbody.lock()->velocity);
						if(force < 0.0f)
							a->rigidbody.lock()->velocity -= normal * force;
					}

					rbA->SetWorldPosition(position);
					a->Update();
				}
				continue;
			}
		}
		// �ӂƂ̔���
		{
			bool hit = false;
			// �܂��������ĂȂ��ꍇ���Ɋe�ӂƏՓ˂��Ă��邩����
			for (int i = 0; i < 3; i++)
			{
				Vector3 h,hNor;
				float def;

				h = Point_VS_Line(SC, face.point[i], face.point[(i + 1) % 3]);
				def = aRadius - h.Length();
				if (def > 0)
				{
					hNor = h.Normalized();
					// �ӂƋ��̒��S�̐����x�N�g�������ʂ̖@���Ɗp�x�����ȏ�Ȃ瓖�����Ă��Ȃ��Ɣ��肷��
					float deg = Mathf::RadToDeg(Mathf::ACosf(Vector3::Dot(hNor, face.normal)));
					if (deg >= 45.0f) continue;

					hit = true;
					// �ǂ��炩���g���K�[�I���Ȃ瓖���������Ƃ����m�点��
					if (a->isTrigger || b->isTrigger) break;

					// �����o������
					bool isRigidA = !a->rigidbody.expired();
					bool isRigidB = !b->rigidbody.expired();
					// �Е��̃��W�b�h�{�f�B���Z�b�g����Ă�����
					if (isRigidA && !isRigidB)
					{
						Transform *rbA = a->rigidbody.lock()->transform.lock().get();
						Vector3 position = rbA->GetWorldPosition();
						float defW = def * per;
						// �@�������[���h��Ԃɕϊ�
						Matrix4 noPosMat = b->worldMatrix;
						noPosMat.matrix(0, 3) = 0.0f; noPosMat.matrix(1, 3) = 0.0f; noPosMat.matrix(2, 3) = 0.0f;
						Vector3 normal = noPosMat * face.normal;
						normal.Normalize();

						//float gravityAngle = Mathf::RadToDeg(Mathf::ACosf(Vector3::Dot(Vector3::up(), normal)));
						//// ���̊p�x�ȉ��Ȃ犊��Ȃ�
						//if (gravityAngle <= 45.1f)
						//{
						//	float pushForce = def;

						//	if (CP_Dot > 0)	// �|���S���̕\���ɂ���ꍇ
						//		position.y += pushForce;
						//	else  // �|���S���̗����ɂ���ꍇ
						//		position.y += pushForce + (a->radius * def);

						//	if (a->rigidbody.lock()->velocity.y < 0.0f)
						//		a->rigidbody.lock()->velocity.y = 0.0f;
						//}
						//else
						{
							if (CP_Dot > 0)	// �|���S���̕\���ɂ���ꍇ
							{
								SC += (normal * def);
								position += (normal * defW);
							}
							else  // �|���S���̗����ɂ���ꍇ
							{
								SC += normal * (def + a->radius);
								position += normal * (defW + a->radius);
							}

							// �����o�������̋t�ɓ����Ă���͂�ł�����
							float force = Vector3::Dot(normal, a->rigidbody.lock()->velocity);
							if (force < 0.0f)
								a->rigidbody.lock()->velocity -= normal * force;
						}

						rbA->SetWorldPosition(position);
						a->Update();
						break;
					}
				}
			}

			isHit = hit;
		}
		// ���_�̓����蔻��( �Ȃ��Ă��ꉞ���삷�� )
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

			//	// �ӂƋ��̒��S�̐����x�N�g�������ʂ̖@���Ɗp�x�����ȏ�Ȃ瓖�����Ă��Ȃ��Ɣ��肷��
			//	float deg = Mathf::RadToDeg(Mathf::ACosf(Vector3::Dot(hNor, face.normal)));
			//	if (deg >= 45.0f) continue;

			//	isHit = true;
			//	// �ǂ��炩���g���K�[�I���Ȃ瓖���������Ƃ����m�点��
			//	if (a->isTrigger || b->isTrigger) break;

			//	// �����o������
			//	bool isRigidA = !a->rigidbody.expired();
			//	bool isRigidB = !b->rigidbody.expired();
			//	// �Е��̃��W�b�h�{�f�B���Z�b�g����Ă�����
			//	if (isRigidA && !isRigidB)
			//	{
			//		Transform *rbA = a->rigidbody.lock()->transform.lock().get();
			//		Vector3 position = rbA->GetWorldPosition();
			//		def *= per;
			//		// �@�������[���h��Ԃɕϊ�
			//		Matrix4 noPosMat = b->worldMatrix;
			//		noPosMat.matrix(0, 3) = 0.0f; noPosMat.matrix(1, 3) = 0.0f; noPosMat.matrix(2, 3) = 0.0f;
			//		Vector3 normal = noPosMat * face.normal;
			//		normal.Normalize();

			//		float gravityAngle = Mathf::RadToDeg(Mathf::ACosf(Vector3::Dot(Vector3::up(), normal)));
			//		// ���̊p�x�ȉ��Ȃ犊��Ȃ�
			//		if (gravityAngle <= 45.1f)
			//		{
			//			hLen *= per;
			//			float pushForce = hLen * def;

			//			if (CP_Dot > 0)	// �|���S���̕\���ɂ���ꍇ
			//				position.y += pushForce;
			//			else  // �|���S���̗����ɂ���ꍇ
			//				position.y += pushForce + (a->radius * def);

			//			if (a->rigidbody.lock()->velocity.y < 0.0f)
			//				a->rigidbody.lock()->velocity.y = 0.0f;
			//		}
			//		else
			//		{
			//			if (CP_Dot > 0)	// �|���S���̕\���ɂ���ꍇ
			//				position = position + (normal * def);
			//			else  // �|���S���̗����ɂ���ꍇ
			//				position = position + (normal * (def + a->radius));

			//			// �����o�������̋t�ɓ����Ă���͂�ł�����
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
	float t = Vector3::Dot(v1.Normalized(), v2) / v1.Length();	// �_��������ɐ����ɉ����ۂ̊���

	if (t < 0.0f)
		h = v2;
	else if (t > 1.0f)
		h = p - lp2;
	else
		h = -(v1 * t) + v2;

	return h;
}
