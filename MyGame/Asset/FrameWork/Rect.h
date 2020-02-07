#ifndef __RECT____H____
#define __RECT____H____

#include  "../Cereal/Common.h"

namespace FrameWork
{
	struct Rect
	{
		Rect() {}
		Rect(int left, int right, int top, int bottom) :left(left), right(right), top(top), bottom(bottom) {}
		int left;
		int top;
		int right;
		int bottom;

	private:
		friend cereal::access;
		template<class Archive>
		void serialize(Archive & archive)
		{
			archive(CEREAL_NVP(left), CEREAL_NVP(top), CEREAL_NVP(right), CEREAL_NVP(bottom));
		}
	};

	struct Rectf
	{
		Rectf() {}
		Rectf(float left, float right, float top, float bottom) :left(left), right(right), top(top), bottom(bottom) {}
		float left;
		float top;
		float right;
		float bottom;

	private:
		friend cereal::access;
		template<class Archive>
		void serialize(Archive & archive)
		{
			archive(CEREAL_NVP(left), CEREAL_NVP(top), CEREAL_NVP(right), CEREAL_NVP(bottom));
		}
	};
}

#endif // !__RECT____H____