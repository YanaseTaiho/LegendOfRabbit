#ifndef __COLOR__H__
#define __COLOR__H__

#include  "../Cereal/Common.h"

namespace FrameWork
{
	struct Color
	{
	private:
		friend cereal::access;
		template<class Archive>
		void serialize(Archive & archive)
		{
			archive(CEREAL_NVP(r), CEREAL_NVP(g), CEREAL_NVP(b), CEREAL_NVP(a));
		}

	public:
		Color() : r(1.0f), g(1.0f), b(1.0f), a(1.0f) {}
		Color(float _r, float _g, float _b, float _a)
			: r(_r), g(_g), b(_b), a(_a) {}
		Color(float _r, float _g, float _b)
			: r(_r), g(_g), b(_b) {}

		Color operator* (const float & value) const { return Color(r * value, g * value, b * value, a * value); }
		Color & operator*= (const float & value) { r *= value; g *= value; b *= value; a *= value; return *this; }
		bool operator== (const Color & value) const { return r == value.r && g == value.g && b == value.b && a == value.a; }
		bool operator!= (const Color & value) const { return r != value.r || g != value.g || b != value.b || a != value.a; }
		operator float* () const { return (float *)&r; }
		operator const float* () const { return (const float *)&r; }

		float r;
		float g;
		float b;
		float a;

		static Color white() { return Color(1.0f, 1.0f, 1.0f, 1.0f); }
		static Color black() { return Color(0.0f, 0.0f, 0.0f, 1.0f); }
		static Color red() { return Color(1.0f, 0.0f, 0.0f, 1.0f); }
		static Color blue() { return Color(0.0f, 0.0f, 1.0f, 1.0f); }
		static Color green() { return Color(0.0f, 1.0f, 0.0f, 1.0f); }
		static Color cyan() { return Color(0.0f, 1.0f, 1.0f, 1.0f); }
		static Color yellow() { return Color(1.0f, 1.0f, 0.0f, 1.0f); }
		static Color orange() { return Color(1.0f, 0.5f, 0.0f, 1.0f); }
	};
}

#endif // !__COLOR__H__

