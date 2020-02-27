#ifndef _SPIKERABBIT_ANIMATIONCONTROLLER_H_
#define _SPIKERABBIT_ANIMATIONCONTROLLER_H_

#include "FrameWork/Animation/AnimationController.h"
#include "Main/main.h"

using namespace FrameWork;

class SpikeRabbitAnimationController : public AnimationController
{
private:
	friend cereal::access;
	template<class Archive>
	void save(Archive & archive) const
	{
		archive(cereal::base_class<AnimationController>(this));
	}
	template<class Archive>
	void load(Archive & archive)
	{
		archive(cereal::base_class<AnimationController>(this));
	}
public:
	SpikeRabbitAnimationController() {}
	~SpikeRabbitAnimationController() {}

	void Initialize() override;
};

CEREAL_REGISTER_TYPE(SpikeRabbitAnimationController)

#endif // !_TESTANIMATIONCONTROLLER_H_

