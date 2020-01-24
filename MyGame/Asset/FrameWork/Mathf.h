#ifndef _MATHF_H_
#define _MATHF_H_

#include <math.h>
#include <DirectXMath.h>
using namespace DirectX;

namespace FrameWork
{
#define DegreeToRadian (0.0174532924f)
#define RadianToDegree (57.29578f)

	struct Mathf
	{
		template <typename T>
		static T Clamp(T value, T min, T max)
		{
			if (value <= min) { return min; }
			else if (value >= max) { return max; }
			else { return value; }
		}
		template <typename T>
		static T Loop(T value, T min, T max)
		{
			if (value < min) { return max; }
			else if (value > max) { return min; }
			else { return value; }
		}
		// 0.0f �` 1.0f �͈͓̔��ɂȂ�悤�ɂ���
		static float Clamp01(float value)
		{
			if (value <= 0.0f) { return 0.0f; }
			else if (value >= 1.0f) { return 1.0f; }
			else { return value; }
		}
		template <typename T>
		static T Lerp(T from, T to, float t)
		{
			t = Clamp01(t);
			return (T)(from * (1.0f - t) + to * t);
		}

		static float Lerp(float from, float to, float t)
		{
			return from * (1.0f - t) + to * t;
		}

		static float Min(float value, float min) { return (value < min) ? min : value; }
		static float Max(float value, float max) { return (value > max) ? max : value; }

		static float Sinf(float value) { return sinf(value); }
		static float ASinf(float value) { return asinf(value); }
		// ���x�������邪�������x������
		static float SinEst(float value) { return XMScalarSinEst(value); }

		static float Cosf(float value) { return cosf(value); }
		static float ACosf(float value) { return acosf(value); }
		// ���x�������邪�������x������
		static float CosEst(float value) { return XMScalarCosEst(value); }

		static float Tanf(float value) { return tanf(value); }
		static float ATanf(float value) { return atanf(value); }
		static float ATan2f(float x, float y) { return atan2f(y, x); }

		// �p�x���烉�W�A���p�ɕϊ�
		static float DegToRad(float deg) { return (deg * DegreeToRadian); }
		// ���W�A���p����p�x�ɕϊ�
		static float RadToDeg(float rad) { return (rad * RadianToDegree); }
		// �p�x���烉�W�A���p�ɕϊ�
		static float DegToRad() { return DegreeToRadian; }
		// ���W�A���p����p�x�ɕϊ�
		static float RadToDeg() { return RadianToDegree; }

		// ��Βl��Ԃ�
		static float Absf(float value) { return fabsf(value); }
		// value��squared�悵�����l��Ԃ�
		static float Powf(float value, float squared) { return powf(value, squared); }
		// ��������Ԃ�
		static float Sqrtf(float value) { return sqrtf(value); }
		// value�����F+1.0f,���F-1.0f,�[���F0.0f��Ԃ�
		static float Signf(float value) 
		{ 
			if (value > 0.0f) return 1.0f;
			if (value < 0.0f) return -1.0f;

			return 0.0f;
		}
	};
}

#endif // !_MATHF_H_

