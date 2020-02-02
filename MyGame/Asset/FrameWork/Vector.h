#ifndef _VECTOR_H_
#define _VECTOR_H_

#include "Mathf.h"
#include  "../Cereal/Common.h"

namespace FrameWork
{
	class Vector2
	{
	private:
		friend cereal::access;
		template<class Archive>
		void serialize(Archive & archive)
		{
			archive(CEREAL_NVP(x), CEREAL_NVP(y));
		}
	
	public:
		float x, y;

		Vector2() : x(0.0f), y(0.0f) {}
		Vector2(float x, float y) : x(x), y(y) {}
		Vector2(XMFLOAT2 f) : x(f.x), y(f.y) {}

		Vector2& operator= (const Vector2& vector2) { x = vector2.x; y = vector2.y; return *this; }
		Vector2& operator= (const XMFLOAT2& f) { x = f.x; y = f.y; return *this; }
		Vector2& operator+= (const Vector2& vector2) { x += vector2.x; y += vector2.y; return *this; }
		Vector2& operator-= (const Vector2& vector2) { x -= vector2.x; y -= vector2.y; return *this; }
		Vector2& operator*= (const Vector2& vector2) { x *= vector2.x; y *= vector2.y; return *this; }
		Vector2& operator/= (const Vector2& vector2) { x /= vector2.x; y /= vector2.y; return *this; }
		Vector2& operator*= (const float Float) { x *= Float; y *= Float; return *this; }
		Vector2& operator/= (const float Float) { x /= Float; y /= Float; return *this; }
		Vector2 operator+ (const Vector2 & vector2) const
		{
			Vector2 v = Vector2(x, y);
			v.x += vector2.x; v.y += vector2.y;
			return v;
		}
		Vector2 operator- (const Vector2 & vector2) const
		{
			Vector2 v = Vector2(x, y);
			v.x -= vector2.x; v.y -= vector2.y;
			return v;
		}
		Vector2 operator* (const float & Float) const
		{
			Vector2 v = Vector2(x, y);
			v *= Float;
			return v;
		}
		Vector2 operator/ (const float & Float) const
		{
			Vector2 v = Vector2(x, y);
			v /= Float;
			return v;
		}
		bool operator== (const Vector2& vector2) const { return (x == vector2.x) && (y == vector2.y); }
		bool operator!= (const Vector2& vector2) const { return !(*this == vector2); }
		Vector2 operator-() const
		{
			return Vector2(-x, -y);
		}
		Vector2 operator+() const
		{
			return Vector2(x, y);
		}
		operator float* () const
		{
			return (float *)&x;
		}
		operator const float* () const
		{
			return (const float *)&x;
		}


		float Length() const { return Mathf::Sqrtf(x * x + y * y); }
		// 長さの比較専用
		float LengthSq() const { return x * x + y * y; }

		// 自身を正規化する
		Vector2 Normalize()
		{
			float value = Length();
			if (value == 1.0f) return *this; // 既に正規化済みなら何もしない
			if (value == 0.0f) return zero();
			*this /= value;
			return *this;
		}

		// 自身を正規化した値を返す
		Vector2 Normalized() const
		{
			return Normalize(*this);
		}

		// 線形補間 value( 0.0f ～ 1.0f )
		static Vector2 Lerp(const Vector2 & from, const Vector2 & to, float value)
		{
			value = Mathf::Clamp01(value);
			return from * (1.0f - value) + to * value;
		}

		// 正規化した値を返す
		static Vector2 Normalize(const Vector2 & vec2)
		{
			float value = vec2.Length();
			if (value == 1.0f) return vec2; // 既に正規化済みなら何もしない
			if (value == 0.0f) return zero();
			return (vec2 / value);
		}

		// 内積
		static float Dot(const Vector2 & vec1, const Vector2 & vec2)
		{
			return (vec1.x * vec2.x + vec1.y * vec2.y);
		}

		// 外積
		static float Cross(const Vector2 & vec1, const Vector2 & vec2)
		{
			return (vec1.x * vec2.y - vec1.y * vec2.x);
		}

		// Vector2(1.0f, 1.0f);
		static Vector2 one() { return Vector2(1.0f, 1.0f); }
		// Vector2(0.0f, 0.0f);
		static Vector2 zero() { return Vector2(0.0f, 0.0f); }
		// Vector2(0.0f, 1.0f);
		static Vector2 up() { return Vector2(0.0f, 1.0f); }
		// Vector2(0.0f, -1.0f);
		static Vector2 down() { return Vector2(0.0f, -1.0f); }
		// Vector2(1.0f, 0.0f);
		static Vector2 right() { return Vector2(1.0f, 0.0f); }
		// Vector2(-1.0f, 0.0f);
		static Vector2 left() { return Vector2(-1.0f, 0.0f); }
	};

	inline Vector2 operator* (const float f, const class Vector2 & v)
	{
		return Vector2(f * v.x, f * v.y);
	}

	inline Vector2 operator/ (const float f, const class Vector2 & v)
	{
		return Vector2(f / v.x, f / v.y);
	}

	class Vector3
	{
	private:
		friend cereal::access;
		template<class Archive>
		void serialize(Archive & archive)
		{
			archive(CEREAL_NVP(x), CEREAL_NVP(y), CEREAL_NVP(z));
		}

	public:
		float x, y, z;

		Vector3() : x(0.0f), y(0.0f), z(0.0f) {}
		Vector3(float x, float y, float z) : x(x), y(y), z(z) {}
		Vector3(const XMFLOAT3& float3) { *this = float3; }

		Vector3& operator= (const Vector3& vector3) { x = vector3.x; y = vector3.y; z = vector3.z; return *this; }
		Vector3& operator= (const XMFLOAT3& float3) { x = float3.x; y = float3.y; z = float3.z; return *this; }
		Vector3& operator+= (const Vector3& vector3) { x += vector3.x; y += vector3.y; z += vector3.z; return *this; }
		Vector3& operator-= (const Vector3& vector3) { x -= vector3.x; y -= vector3.y; z -= vector3.z; return *this; }
		Vector3& operator*= (const Vector3& vector3) { x *= vector3.x; y *= vector3.y; z *= vector3.z; return *this; }
		Vector3& operator/= (const Vector3& vector3) { x /= vector3.x; y /= vector3.y; z /= vector3.z; return *this; }
		Vector3& operator*= (const float& Float) { x *= Float; y *= Float; z *= Float; return *this; }
		Vector3& operator/= (const float& Float) { x /= Float; y /= Float; z /= Float; return *this; }
		Vector3 operator+ (const Vector3 vector3) const
		{
			return Vector3(x + vector3.x, y + vector3.y, z + vector3.z);
		}
		Vector3 operator- (const Vector3 vector3) const
		{
			return Vector3(x - vector3.x, y - vector3.y, z - vector3.z);;
		}
		Vector3 operator* (const Vector3 vector3) const
		{
			return Vector3(x * vector3.x, y * vector3.y, z * vector3.z);
		}
		Vector3 operator/ (const Vector3 vector3) const
		{
			return Vector3(x / vector3.x, y / vector3.y, z / vector3.z);
		}
		Vector3 operator* (const float Float) const
		{
			return Vector3(x * Float, y * Float, z * Float);
		}
		Vector3 operator/ (const float Float) const
		{
			return Vector3(x / Float, y / Float, z / Float);;
		}
		bool operator== (const Vector3& vector3) const { return (x == vector3.x) && (y == vector3.y) && (z == vector3.z); }
		bool operator!= (const Vector3& vector3) const { return (x != vector3.x) || (y != vector3.y) || (z != vector3.z); }

		Vector3 operator-() const
		{
			return Vector3(-x, -y, -z);
		}
		Vector3 operator+() const
		{
			return Vector3(x, y, z);
		}
		operator float* () const
		{
			return (float *)&x;
		}
		operator const float* () const
		{
			return (const float *)&x;
		}

		// XMFLOAT3として返す
		XMFLOAT3 XMFLOAT() const { return XMFLOAT3(x, y, z); }

		// ベクトルの長さ取得
		float Length() const { return Mathf::Sqrtf(x * x + y * y + z * z); }
		// ベクトルの長さの比較専用
		float LengthSq() const { return x * x + y * y + z * z; }


		// 自身を正規化する
		Vector3 Normalize() 
		{ 
			float value = Length(); 
			if (value == 1.0f) return *this; // 既に正規化済みなら何もしない
			if (value == 0.0f) return zero();
			*this /= value;
			return *this;
		}

		// 自身の正規化した値を返す
		Vector3 Normalized() const
		{
			return Normalize(*this);
		}

		// 自身との内積
		float Dot(const Vector3& vec) const
		{
			return Dot(*this, vec);
		}

		// 自身との外積
		Vector3 Cross(const Vector3& vec) const
		{
			return Cross(*this, vec);
		}

		// 線形補間 value( 0.0f ～ 1.0f )
		static Vector3 Lerp(const Vector3& from, const Vector3& to, float value)
		{
			value = Mathf::Clamp01(value);
			return from * (1.0f - value) + to * value;
		}

		// 正規化した値を返す
		static Vector3 Normalize(const Vector3& vector3)
		{
			float value = vector3.Length();
			if (value == 0.0f) return zero();
			return (vector3 / value);
		}

		// 内積
		static float Dot(const Vector3& vec1, const Vector3& vec2)
		{
			return (vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z);
		}

		// 外積
		static Vector3 Cross(const Vector3& vec1, const Vector3& vec2)
		{
			Vector3 vec;
			double x1, y1, z1, x2, y2, z2;

			x1 = (double)(vec1.x);
			y1 = (double)(vec1.y);
			z1 = (double)(vec1.z);
			x2 = (double)(vec2.x);
			y2 = (double)(vec2.y);
			z2 = (double)(vec2.z);

			vec.x = (float)(y1 * z2 - z1 * y2);
			vec.y = (float)(z1 * x2 - x1 * z2);
			vec.z = (float)(x1 * y2 - y1 * x2);

			return vec;
		}

		//（ベクトル3法線、ベクトル3接線）
		static Vector3 OrthoNormalize(Vector3* normal, Vector3* tangent)
		{
			normal->Normalize();
			Vector3 norm = *normal;
			Vector3 tan = tangent->Normalize();

			*tangent = tan - (norm * Vector3::Dot(norm, tan));
			tangent->Normalize();
			return *tangent;
		}

		// return Vector3(1.0f, 1.0f, 1.0f);
		static Vector3 one() { return Vector3(1.0f, 1.0f, 1.0f); }
		// return Vector3(0.0f, 0.0f, 0.0f);
		static Vector3 zero() { return Vector3(0.0f, 0.0f, 0.0f); }
		// return Vector3(0.0f, 1.0f, 0.0f);
		static Vector3 up() { return Vector3(0.0f, 1.0f, 0.0f); }
		// return Vector3(0.0f, -1.0f, 0.0f);
		static Vector3 down() { return Vector3(0.0f, -1.0f, 0.0f); }
		// return Vector3(1.0f, 0.0f, 0.0f);
		static Vector3 right() { return Vector3(1.0f, 0.0f, 0.0f); }
		// return Vector3(-1.0f, 0.0f, 0.0f);
		static Vector3 left() { return Vector3(-1.0f, 0.0f, 0.0f); }
		// return Vector3(0.0f, 0.0f, 1.0f);
		static Vector3 forward() { return Vector3(0.0f, 0.0f, 1.0f); }
		// return Vector3(0.0f, 0.0f, -1.0f);
		static Vector3 back() { return Vector3(0.0f, 0.0f, -1.0f); }
	};

	inline Vector3 operator* (const float f, const class Vector3 & v)
	{
		return Vector3(f * v.x, f * v.y, f * v.z);
	}

	inline Vector3 operator/ (const float f, const class Vector3 & v)
	{
		return Vector3(f / v.x, f / v.y, f / v.z);
	}
}

#endif // !_VECTOR_H_

