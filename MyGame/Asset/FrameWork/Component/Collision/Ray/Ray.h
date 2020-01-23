#ifndef _RAY_H_
#define _RAY_H_

#include "../../../Vector.h"

namespace FrameWork
{
	struct Ray
	{
		Vector3 start;
		Vector3 end;

		Ray() {}
		Ray(Vector3 start, Vector3 end) : start(start), end(end) {}
		Ray(Vector3 start, Vector3 dir, float length) : start(start), end(start + (dir * length)){}

		void Set(Vector3 start, Vector3 end)
		{
			this->start = start;
			this->end = end;
		}
		void Set(Vector3 start, Vector3 dir, float length)
		{
			this->start = start;
			this->end = start + (dir.Normalize() * length);
		}

	private:
		friend cereal::access;
		template<class Archive>
		void serialize(Archive & archive)
		{
			archive(CEREAL_NVP(start), CEREAL_NVP(end));
		}
	};
}

#endif // !_RAY_H_

