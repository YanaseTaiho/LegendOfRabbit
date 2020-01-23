#ifndef _TAKES_VECTOR3_H_
#define _TAKES_VECTOR3_H_

#include "Takes.h"
#include "../../Vector.h"

namespace FrameWork
{
	class TakesVector3 : public Takes<Vector3>
	{
	private:
		Vector3 BlendTake(const Vector3 & a, const Vector3 & b, float weight) override
		{
			return Vector3::Lerp(a, b, weight);
		}
	private:
		friend cereal::access;
		template<class Archive>
		void serialize(Archive & archive)
		{
			archive(cereal::make_nvp("Takes<Vector3>", cereal::base_class<Takes<Vector3>>(this)));
		}
	};
}

#endif // !_TAKES_VECTOR3_H_

