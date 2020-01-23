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
		// これは直接使わない方がいい
		Eigen::Quaternionf quaternion;

		Quaternion();
		Quaternion(float x, float y, float z, float w);
		Quaternion(float x, float y, float z);	// オイラー角で初期化
		Quaternion(Vector3 v);	// オイラー角で初期化
		Quaternion(Vector3 axis, float angle);	// 軸( axis ) 回転角度( angle )

		Quaternion& operator*= (const Quaternion& q);
		Quaternion operator* (const Quaternion& q);
		bool operator== (const Quaternion& q);
		bool operator!= (const Quaternion& q);

		// x,y,z,wを直接指定
		Quaternion Set(float x, float y, float z, float w);

		// オイラー角で設定
		Quaternion SetEulerAngles(float x, float y, float z);
		Quaternion SetEulerAngles(Vector3 v);
		// オイラー角を取得
		Vector3 GetEulerAngles();

		// 軸( axis ) 回転角度( angle )
		Quaternion SetAxisAngle(Vector3 axis, float angle);

		// 自身を単位クオータニオン( 実数が1で虚数が0 )に設定
		Quaternion SetIdentity();

		Quaternion Slerp(Quaternion q, float t);
		// 二つのベクトルの間の角度のクオータニオンを求める
		Quaternion SetFormToRotation(Vector3 from, Vector3 to);
		// 目的の方向に向くようにセット
		Quaternion SetLookAt(Vector3 lookAt, Vector3 upDirection = Vector3::up())
		{
			quaternion = LookRotation(lookAt, upDirection).quaternion;
			return *this;
		}

		// 自身を正規化
		Quaternion Normalize();
		// 自身の正規化された値を取得( 自身は変化しない )
		Quaternion Normalized() const;
		// 自身を逆クオータニオンにする( 逆回転させたい場合に使える )
		Quaternion Inverse();
		// 自身の逆クオータニオンを取得( 逆回転させたい場合に使える )
		Quaternion Inversed() const;
		// 自身を共役クオータニオンにする( 逆回転させたい場合に使える( 正規化された時だけなので注意!! ) )
		Quaternion Conjugate();
		// 自身の共役クオータニオンを取得( 逆回転させたい場合に使える( 正規化された時だけなので注意!! ) )
		Quaternion Conjugated() const;

		// 自身との内積
		float Dot(Quaternion q);

		// 引数二つとの内積
		static float Dot(Quaternion q1, Quaternion q2);
		// 逆クオータニオンを返す( 逆回転させたい場合に使える )
		static Quaternion Inverse(Quaternion q);
		// 単位クオータニオン( 実数が1で虚数が0 )
		static Quaternion Identity();
		// 正規化
		static Quaternion Normalize(Quaternion q);
		// 共役クオータニオンを返す( 逆回転させたい場合に使える( 正規化された時だけなので注意!! ) )
		static Quaternion Conjugate(Quaternion q);
		// 二つのベクトルの間の角度のクオータニオンを求める
		static Quaternion FromToRotation(Vector3 from, Vector3 to);
		// 目的の方向に向く
		static Quaternion LookRotation(Vector3 lookAt, Vector3 upDirection = Vector3::up());

		// 二つの回転間の角度（ラジアン）
		static float AngularDistance(Quaternion q1, Quaternion q2);

		static Quaternion EulerAngles(float x, float y, float z);
		static Quaternion EulerAngles(Vector3 v);
		static Quaternion AxisAngle(Vector3 axis, float angle);

		static Quaternion Slerp(Quaternion from, Quaternion to, float t);

		float& x() { return quaternion.x(); }
		float& y() { return quaternion.y(); }
		float& z() { return quaternion.z(); }
		float& w() { return quaternion.w(); }


		//========== 自分で作った奴だけどもう使わない ==========//
		/*
		Quaternion() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
		// 軸( axis ) 回転角度( angle( ラジアン角 ) )
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

		// 軸( axis ) 回転角度( angle( ラジアン角 ) )
		Quaternion SetAxisAngle(Vector3 axis, float angle)
		{
			float t = Mathf::Sinf(angle / 2);
			x = axis.x * t;
			y = axis.y * t;
			z = axis.z * t;
			w = Mathf::Cosf(angle / 2);

			return *this;
		}

		// 軸( axis ) 回転角度( angle( ラジアン角 ) )
		static Quaternion AxisAngle(Vector3 axis, float angle) { Quaternion q; return q.SetAxisAngle(axis, angle); }

		// 内積
		static float Dot(Quaternion q1, Quaternion q2)
		{
			return q1.x * q2.x + q1.y * q2.y + q1.z * q2.z + q1.w * q2.w;
		}

		// 球面線形補間 t( 0.0f ～ 1.0f )
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
		/* //クォータニオンを行列に変換
		
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

