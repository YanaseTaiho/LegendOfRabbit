#ifndef _RABITANIMATIONCONTROLLER_H_
#define _RABITANIMATIONCONTROLLER_H_

#include "FrameWork/Animation/AnimationController.h"
#include "Main/main.h"

using namespace FrameWork;

class RabitAnimationController : public AnimationController
{
private:
	friend cereal::access;
	template<class Archive>
	void save(Archive & archive, std::uint32_t const version) const
	{
		archive(cereal::make_nvp("AnimationController", cereal::base_class<AnimationController>(this)));
	}
	friend cereal::access;
	template<class Archive>
	void load(Archive & archive, std::uint32_t const version)
	{
		archive(cereal::make_nvp("AnimationController", cereal::base_class<AnimationController>(this)));
	}

public:
	RabitAnimationController() {};
	~RabitAnimationController() {};

	void Initialize() override;
};

CEREAL_CLASS_VERSION(RabitAnimationController, 0)
CEREAL_REGISTER_TYPE(RabitAnimationController)

#endif // !_RABITANIMATIONCONTROLLER_H_

