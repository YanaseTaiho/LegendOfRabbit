#ifndef _TAKES_QUATERNION_H_
#define _TAKES_QUATERNION_H_

#include "Takes.h"
#include "../../Quaternion.h"

namespace FrameWork
{
	class TakesQuaternion : public Takes<Quaternion>
	{
	private:
		Quaternion BlendTake(const Quaternion & a, const Quaternion & b, float weight) override
		{
			return Quaternion::Slerp(a, b, weight);
		}
	private:
		friend cereal::access;
		template<class Archive>
		void serialize(Archive & archive)
		{
			archive(cereal::make_nvp("Takes<Quaternion>", cereal::base_class<Takes<Quaternion>>(this)));
		}
	};
}

#endif // !_TAKES_QUATERNION_H_
