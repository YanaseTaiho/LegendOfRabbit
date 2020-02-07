#ifndef _QUATERNION_H_
#define _QUATERNION_H_

#include <Eigen/Geometry>
#include "Vector.h"

#include "../Cereal/Common.h"

namespace FrameWork
{
	class Quaternion
	{
	private:
		friend cereal::access;
		template<class Archive>
		void serialize(Archive & archive)
		{
			archive(cereal::make_nvp("x", quaternion.x()),
				cereal::make_nvp("y", quaternion.y()),
				cereal::make_nvp("z", quaternion.z()),
				cereal::make_nvp("w", quaternion.w()));
		}

	public:
		// ����͒��ڎg��Ȃ���������
		Eigen::Quaternionf quaternion;

		Quaternion();
		Quaternion(float x, float y, float z, float w);
		Quaternion(float x, float y, float z);	// �I�C���[�p�ŏ�����
		Quaternion(Vector3 v);	// �I�C���[�p�ŏ�����
		Quaternion(Vector3 axis, float angle);	// ��( axis ) ��]�p�x( angle )

		Quaternion& operator*= (const Quaternion& q);
		Quaternion operator* (const Quaternion& q);
		bool operator== (const Quaternion& q);
		bool operator!= (const Quaternion& q);

		// x,y,z,w�𒼐ڎw��
		Quaternion Set(float x, float y, float z, float w);

		// �I�C���[�p�Őݒ�
		Quaternion SetEulerAngles(float x, float y, float z);
		Quaternion SetEulerAngles(Vector3 v);
		// �I�C���[�p���擾
		Vector3 GetEulerAngles();

		// ��( axis ) ��]�p�x( angle )
		Quaternion SetAxisAngle(Vector3 axis, float angle);

		// ���g��P�ʃN�I�[�^�j�I��( ������1�ŋ�����0 )�ɐݒ�
		Quaternion SetIdentity();

		Quaternion Slerp(const Quaternion & q, float t) const;
		// ��̃x�N�g���̊Ԃ̊p�x�̃N�I�[�^�j�I�������߂�
		Quaternion SetFormToRotation(Vector3 from, Vector3 to);
		// �ړI�̕����Ɍ����悤�ɃZ�b�g
		Quaternion SetLookAt(Vector3 lookAt, Vector3 upDirection = Vector3::up())
		{
			quaternion = LookRotation(lookAt, upDirection).quaternion;
			return *this;
		}

		// ���g�𐳋K��
		Quaternion Normalize();
		// ���g�̐��K�����ꂽ�l���擾( ���g�͕ω����Ȃ� )
		Quaternion Normalized() const;
		// ���g���t�N�I�[�^�j�I���ɂ���( �t��]���������ꍇ�Ɏg���� )
		Quaternion Inverse();
		// ���g�̋t�N�I�[�^�j�I�����擾( �t��]���������ꍇ�Ɏg���� )
		Quaternion Inversed() const;
		// ���g�������N�I�[�^�j�I���ɂ���( �t��]���������ꍇ�Ɏg����( ���K�����ꂽ�������Ȃ̂Œ���!! ) )
		Quaternion Conjugate();
		// ���g�̋����N�I�[�^�j�I�����擾( �t��]���������ꍇ�Ɏg����( ���K�����ꂽ�������Ȃ̂Œ���!! ) )
		Quaternion Conjugated() const;

		// ���g�Ƃ̓���
		float Dot(Quaternion q);

		// ������Ƃ̓���
		static float Dot(Quaternion q1, Quaternion q2);
		// �t�N�I�[�^�j�I����Ԃ�( �t��]���������ꍇ�Ɏg���� )
		static Quaternion Inverse(Quaternion q);
		// �P�ʃN�I�[�^�j�I��( ������1�ŋ�����0 )
		static Quaternion Identity();
		// ���K��
		static Quaternion Normalize(Quaternion q);
		// �����N�I�[�^�j�I����Ԃ�( �t��]���������ꍇ�Ɏg����( ���K�����ꂽ�������Ȃ̂Œ���!! ) )
		static Quaternion Conjugate(Quaternion q);
		// ��̃x�N�g���̊Ԃ̊p�x�̃N�I�[�^�j�I�������߂�
		static Quaternion FromToRotation(Vector3 from, Vector3 to);
		// �ړI�̕����Ɍ���
		static Quaternion LookRotation(Vector3 lookAt, Vector3 upDirection = Vector3::up());

		// ��̉�]�Ԃ̊p�x�i���W�A���j
		static float AngularDistance(Quaternion q1, Quaternion q2);

		static Quaternion EulerAngles(float x, float y, float z);
		static Quaternion EulerAngles(Vector3 v);
		static Quaternion AxisAngle(Vector3 axis, float angle);

		static Quaternion Slerp(Quaternion from, Quaternion to, float t);

		float& x() { return quaternion.x(); }
		float& y() { return quaternion.y(); }
		float& z() { return quaternion.z(); }
		float& w() { return quaternion.w(); }


		//========== Eigen���g���̂ł����g��Ȃ� ==========//
		/*
		Quaternion() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
		// ��( axis ) ��]�p�x( angle( ���W�A���p ) )
		Quaternion(Vector3 axis, float angle) { SetAxisAngle(axis, angle); }
		Quaternion(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

		Quaternion& operator*= (const Quaternion& q)
		{
			float pw, px, py, pz;
			float qw, qx, qy, qz;

			pw = this->w; px = this->x; py = this->y; pz = this->z;
			qw = q.w; qx = q.x; qy = q.y; qz = q.z;

			this->w = pw * qw - px * qx - py * qy - pz * qz;
			this->x = pw * qx + px * qw + py * qz - pz * qy;
			this->y = pw * qy - px * qz + py * qw + pz * qx;
			this->z = pw * qz + px * qy - py * qx + pz * qw;

			return *this;
		}
		Quaternion operator* (const Quaternion& q)
		{
			Quaternion q1 = *this;
			q1 *= q;
			return q1;
		}
		Quaternion& operator*= (const float& s) { x *= s; y *= s; z *= s; w *= s; return *this; }
		Quaternion operator* (const float& s) 
		{ 
			Quaternion q = *this;
			q *= s;
			return q;
		}

		bool operator== (const Quaternion& q) { return (x == q.x) && (y == q.y) && (z == q.z) && (w == q.w); }
		bool operator!= (const Quaternion& q) { return !(*this == q); }

		Quaternion Normalized()
		{
			float n = 1.0f / Mathf::Sqrtf(x * x + y * y + z * z + w * w);
			*this *= n;
			return *this;
		}

		// ��( axis ) ��]�p�x( angle( ���W�A���p ) )
		Quaternion SetAxisAngle(Vector3 axis, float angle)
		{
			float t = Mathf::Sinf(angle / 2);
			x = axis.x * t;
			y = axis.y * t;
			z = axis.z * t;
			w = Mathf::Cosf(angle / 2);

			return *this;
		}

		// ��( axis ) ��]�p�x( angle( ���W�A���p ) )
		static Quaternion AxisAngle(Vector3 axis, float angle) { Quaternion q; return q.SetAxisAngle(axis, angle); }

		// ����
		static float Dot(Quaternion q1, Quaternion q2)
		{
			return q1.x * q2.x + q1.y * q2.y + q1.z * q2.z + q1.w * q2.w;
		}

		// ���ʐ��`��� t( 0.0f �` 1.0f )
		static Quaternion Slerp(Quaternion q1, Quaternion q2, float t)
		{
			float dot = Dot(q1, q2);
			float theta = Mathf::ACosf(dot);

			if (theta < 0.0f)
			{
				theta = -theta;
			}

			float st = Mathf::Sinf(theta);

			if (st == 0.0f)
			{
				return q1;
			}

			t = Mathf::Clamp01(t);
			float sut = Mathf::Sinf(theta * t);
			float sout = Mathf::Sinf(theta * (1.0f - t));

			float coeff1 = sout / st;
			float coeff2 = sut / st;

			float x = coeff1 * q1.x + coeff2 * q2.x;
			float y = coeff1 * q1.y + coeff2 * q2.y;
			float z = coeff1 * q1.z + coeff2 * q2.z;
			float w = coeff1 * q1.w + coeff2 * q2.w;

			return Quaternion(x, y, z, w);
		}
		*/
		/* //�N�H�[�^�j�I�����s��ɕϊ�
		
		//lpM <= lpQ
		void FUTL_QuatToMatrix(FMatrix *lpM, FQuat *lpQ)
		{
			float qw, qx, qy, qz;
			float x2, y2, z2;
			float xy, yz, zx;
			float wx, wy, wz;

			qw = lpQ->w; qx = lpQ->x; qy = lpQ->y; qz = lpQ->z;

			x2 = 2.0f * qx * qx;
			y2 = 2.0f * qy * qy;
			z2 = 2.0f * qz * qz;

			xy = 2.0f * qx * qy;
			yz = 2.0f * qy * qz;
			zx = 2.0f * qz * qx;

			wx = 2.0f * qw * qx;
			wy = 2.0f * qw * qy;
			wz = 2.0f * qw * qz;

			lpM->m00 = 1.0f - y2 - z2;
			lpM->m01 = xy - wz;
			lpM->m02 = zx + wy;
			lpM->m03 = 0.0f;

			lpM->m10 = xy + wz;
			lpM->m11 = 1.0f - z2 - x2;
			lpM->m12 = yz - wx;
			lpM->m13 = 0.0f;

			lpM->m20 = zx - wy;
			lpM->m21 = yz + wx;
			lpM->m22 = 1.0f - x2 - y2;
			lpM->m23 = 0.0f;

			lpM->m30 = lpM->m31 = lpM->m32 = 0.0f;
			lpM->m33 = 1.0f;
		}
		*/
	};
}

#endif // !_QUATERNION_H_

