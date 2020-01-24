#ifndef _TESTANIMATIONCONTROLLER_H_
#define _TESTANIMATIONCONTROLLER_H_

#include "FrameWork/Animation/AnimationController.h"
#include "Main/main.h"

using namespace FrameWork;

class TestAnimationController : public AnimationController
{
private:
	friend cereal::access;
	template<class Archive>
	void save(Archive & archive) const
	{
		archive(cereal::make_nvp("AnimationController", cereal::base_class<AnimationController>(this)));
		archive(CEREAL_NVP(speed));
	}
	template<class Archive>
	void load(Archive & archive)
	{
		archive(cereal::make_nvp("AnimationController", cereal::base_class<AnimationController>(this)));
		archive(CEREAL_NVP(speed));
	}
public:
	TestAnimationController(std::string animName, float speed = 60.0f) : animName1(animName), speed(speed){}
	TestAnimationController() {}
	~TestAnimationController() {}

	std::string animName1;
	float speed;

	void Initialize() override
	{
		AnimationController::Initialize();

		AddFilter("Normal", [](std::shared_ptr<AnimationFilter> & filter)
		{
			// �X�e�[�g�ǉ�
			auto idle = filter->AddState("Idel");
			// �G���g���[�|�C���g�ݒ�
			filter->SetEntryPoint(idle);
		});
	}
};

CEREAL_REGISTER_TYPE(TestAnimationController)

#endif // !_TESTANIMATIONCONTROLLER_H_

