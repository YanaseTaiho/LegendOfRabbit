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

	void Initialize() override
	{
		AnimationController::Initialize();

		// ステート追加
		auto idle = AddState("Idle");
		auto walk = AddState("Walk");
		auto run = AddState("Run");
		auto roll = AddState("Roll");
		auto rollStop = AddState("RollStop");
		auto jump = AddState("Jump");
		auto fall = AddState("Fall");
		auto land = AddState("Land");

		auto cliff_Jump = AddState("Cliff_Jump");
		auto cliff_Grap = AddState("Cliff_Grap");
		auto cliff_Idle = AddState("Cliff_Idle");
		auto cliff_Up = AddState("Cliff_Up");

		// エントリーポイント設定
		SetEntryPoint(idle);

		// パラメータ追加
		auto walkValue = AddParameterFloat("WalkValue", 0.0f);
		auto jumpTrigger = AddParameterTrigger("JumpTrigger", false);
		auto rollTrigger = AddParameterTrigger("RollTrigger", false);
		auto rollStopTrigger = AddParameterTrigger("RollStopTrigger", false);
		auto isFall = AddParameterBool("IsFall", false);

		auto cliff_Jump_Trigger = AddParameterTrigger("Cliff_Jump_Trigger", false);
		auto isCliff_Grap = AddParameterBool("IsCliff_Grap", false);
		auto cliff_Up_Trigger = AddParameterTrigger("Cliff_Up_Trigger", false);

		// トランジション追加
		
		// Idle
		{
			 AddTransition(idle, walk, [&](std::shared_ptr<AnimationTransition> & transition)
			 {
				 transition->SetOption(0.1f, false);
				 transition->AddConditionFloat(walkValue, Greater, 0.1f);
			 });
			 AddTransition(idle, jump, [&](std::shared_ptr<AnimationTransition> & transition)
			 {
				 transition->SetOption(0.1f, false);
				 transition->AddConditionTrigger(jumpTrigger);
			 });
			 AddTransition(idle, fall, [&](std::shared_ptr<AnimationTransition> & transition)
			 {
				 transition->SetOption(0.2f, false);
				 transition->AddConditionBool(isFall, true);
			 });
			 AddTransition(idle, cliff_Jump, [&](std::shared_ptr<AnimationTransition> & transition)
			 {
				 transition->SetOption(0.03f, false);
				 transition->AddConditionTrigger(cliff_Jump_Trigger);
			 });
		}
		// Walk
		{
			AddTransition(walk, idle, [&](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.2f, false);
				transition->AddConditionFloat(walkValue, Less, 0.1f);
			});
			AddTransition(walk, run, [&](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.3f, false);
				transition->AddConditionFloat(walkValue, Greater, 0.5f);
			});
			AddTransition(walk, jump, [&](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.1f, false);
				transition->AddConditionTrigger(jumpTrigger);
			});
			AddTransition(walk, fall, [&](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.2f, false);
				transition->AddConditionBool(isFall, true);
			});
			AddTransition(walk, roll, [&](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.05f, false);
				transition->AddConditionTrigger(rollTrigger);
			});
			AddTransition(walk, cliff_Jump, [&](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.03f, false);
				transition->AddConditionTrigger(cliff_Jump_Trigger);
			});
		}
		// Run
		{
			AddTransition(run, idle, [&](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.1f, false);
				transition->AddConditionFloat(walkValue, Less, 0.1f);
			});
			AddTransition(run, walk, [&](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.05f, false);
				transition->AddConditionFloat(walkValue, Less, 0.5f);
			});
			AddTransition(run, jump, [&](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.1f, false);
				transition->AddConditionTrigger(jumpTrigger);
			});
			AddTransition(run, fall, [&](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.2f, false);
				transition->AddConditionBool(isFall, true);
			});

			AddTransition(run, roll, [&](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.05f, false);
				transition->AddConditionTrigger(rollTrigger);
			});
		}
		// Roll
		{
			AddTransition(roll, rollStop, [&](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.05f, false);
				transition->AddConditionTrigger(rollStopTrigger);
			});

			AddTransition(roll, idle, [&](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.05f, true);
				transition->AddConditionFloat(walkValue, Less, 0.1f);
			});
			AddTransition(roll, walk, [&](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.1f, true);
				transition->AddConditionFloat(walkValue, Less, 0.5f);
			});
			AddTransition(roll, run, [&](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.1f, true);
				transition->AddConditionFloat(walkValue, Greater, 0.5f);
			});
			AddTransition(roll, jump, [&](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.05f, false);
				transition->AddConditionTrigger(jumpTrigger);
			});
			AddTransition(roll, fall, [&](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.1f, false);
				transition->AddConditionBool(isFall, true);
			});
		}
		// RollStop
		{
			AddTransition(rollStop, idle, [&](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.0f, true);
			});
		}
		// Jump
		{
			AddTransition(jump, cliff_Grap, [&](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.0f, false);
				transition->AddConditionBool(isCliff_Grap, true);
			});
			AddTransition(jump, run, [&](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.1f, false);
				transition->AddConditionBool(isFall, false);
				transition->AddConditionFloat(walkValue, Greater, 0.5f);
			});
			AddTransition(jump, walk, [&](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.1f, false);
				transition->AddConditionBool(isFall, false);
				transition->AddConditionFloat(walkValue, Greater, 0.1f);
			});
			AddTransition(jump, land, [&](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.03f, false);
				transition->AddConditionBool(isFall, false);
			});
		}
		// Fall
		{
			AddTransition(fall, cliff_Grap, [&](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.0f, false);
				transition->AddConditionBool(isCliff_Grap, true);
			});
			AddTransition(fall, run, [&](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.1f, false);
				transition->AddConditionBool(isFall, false);
				transition->AddConditionFloat(walkValue, Greater, 0.5f);
			});				
			AddTransition(fall, walk, [&](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.1f, false);
				transition->AddConditionBool(isFall, false);
				transition->AddConditionFloat(walkValue, Greater, 0.1f);
			});
			AddTransition(fall, land, [&](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.03f, false);
				transition->AddConditionBool(isFall, false);
			});
		}
		// Land
		{
			AddTransition(land, idle, [&](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.0f, true);
			});	
			AddTransition(land, run, [&](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.2f, false);
				transition->AddConditionFloat(walkValue, Greater, 0.5f);
			});
			AddTransition(land, walk, [&](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.2f, false);
				transition->AddConditionFloat(walkValue, Greater, 0.1f);
			});
		}
		// Cliff_Jump
		{
			AddTransition(cliff_Jump, cliff_Grap, [&](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.0f, false);
				transition->AddConditionBool(isCliff_Grap, true);
			});
			AddTransition(cliff_Jump, land, [&](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.1f, true, 0.5f);
				transition->AddConditionBool(isFall, false);
			});
		}
		// Cliff_Grap
		{
			AddTransition(cliff_Grap, cliff_Idle, [&](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.0f, true);
			});
			AddTransition(cliff_Grap, cliff_Up, [&](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.05f, false);
				transition->AddConditionTrigger(cliff_Up_Trigger);
			});
			AddTransition(cliff_Grap, fall, [&](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.1f, false);
				transition->AddConditionBool(isCliff_Grap, false);
			});
		}
		// Cliff_Idle
		{
			AddTransition(cliff_Idle, cliff_Up, [&](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.0f, false);
				transition->AddConditionTrigger(cliff_Up_Trigger);
			});
			AddTransition(cliff_Idle, fall, [&](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.1f, false);
				transition->AddConditionBool(isCliff_Grap, false);
			});
		}
		// Cliff_Up
		{
			AddTransition(cliff_Up, idle, [&](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.0f, true);
			});
		}
	}
};

CEREAL_CLASS_VERSION(RabitAnimationController, 0)
CEREAL_REGISTER_TYPE(RabitAnimationController)

#endif // !_RABITANIMATIONCONTROLLER_H_

