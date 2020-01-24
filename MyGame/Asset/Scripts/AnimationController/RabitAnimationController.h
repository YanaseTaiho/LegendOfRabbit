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

		// パラメータ追加
		auto walkValue = AddParameterFloat("WalkValue", 0.0f);
		auto jumpTrigger = AddParameterTrigger("JumpTrigger", false);
		auto rollTrigger = AddParameterTrigger("RollTrigger", false);
		auto rollStopTrigger = AddParameterTrigger("RollStopTrigger", false);
		auto isFall = AddParameterBool("IsFall", false);

		auto cliff_Jump_Trigger = AddParameterTrigger("Cliff_Jump_Trigger", false);
		auto isCliff_Grap = AddParameterBool("IsCliff_Grap", false);
		auto cliff_Up_Trigger = AddParameterTrigger("Cliff_Up_Trigger", false);

		AddFilter("Normal", [&](std::shared_ptr<AnimationFilter> & filter)
		{
			// ステート追加
			auto idle = filter->AddState("Idle");
			auto walk = filter->AddState("Walk");
			auto run = filter->AddState("Run");
			auto roll = filter->AddState("Roll");
			auto rollStop = filter->AddState("RollStop");
			auto jump = filter->AddState("Jump");
			auto fall = filter->AddState("Fall");
			auto land = filter->AddState("Land");

			auto cliff_Jump = filter->AddState("Cliff_Jump");
			auto cliff_Grap = filter->AddState("Cliff_Grap");
			auto cliff_Idle = filter->AddState("Cliff_Idle");
			auto cliff_Up = filter->AddState("Cliff_Up");

			// エントリーポイント設定
			filter->SetEntryPoint(idle);

			// トランジション追加

			// Idle
			{
				idle->AddTransition(walk, [&](std::shared_ptr<AnimationTransition> & transition)
				{
					transition->SetOption(0.1f, false);
					transition->AddConditionFloat(walkValue, Greater, 0.1f);
				});
				idle->AddTransition(jump, [&](std::shared_ptr<AnimationTransition> & transition)
				{
					transition->SetOption(0.1f, false);
					transition->AddConditionTrigger(jumpTrigger);
				});
				idle->AddTransition(fall, [&](std::shared_ptr<AnimationTransition> & transition)
				{
					transition->SetOption(0.2f, false);
					transition->AddConditionBool(isFall, true);
				});
				idle->AddTransition(cliff_Jump, [&](std::shared_ptr<AnimationTransition> & transition)
				{
					transition->SetOption(0.03f, false);
					transition->AddConditionTrigger(cliff_Jump_Trigger);
				});
			}
			// Walk
			{
				walk->AddTransition(idle, [&](std::shared_ptr<AnimationTransition> & transition)
				{
					transition->SetOption(0.2f, false);
					transition->AddConditionFloat(walkValue, Less, 0.1f);
				});
				walk->AddTransition(run, [&](std::shared_ptr<AnimationTransition> & transition)
				{
					transition->SetOption(0.3f, false);
					transition->AddConditionFloat(walkValue, Greater, 0.5f);
				});
				walk->AddTransition(jump, [&](std::shared_ptr<AnimationTransition> & transition)
				{
					transition->SetOption(0.1f, false);
					transition->AddConditionTrigger(jumpTrigger);
				});
				walk->AddTransition(fall, [&](std::shared_ptr<AnimationTransition> & transition)
				{
					transition->SetOption(0.2f, false);
					transition->AddConditionBool(isFall, true);
				});
				walk->AddTransition(roll, [&](std::shared_ptr<AnimationTransition> & transition)
				{
					transition->SetOption(0.05f, false);
					transition->AddConditionTrigger(rollTrigger);
				});
				walk->AddTransition(cliff_Jump, [&](std::shared_ptr<AnimationTransition> & transition)
				{
					transition->SetOption(0.03f, false);
					transition->AddConditionTrigger(cliff_Jump_Trigger);
				});
			}
			// Run
			{
				run->AddTransition(idle, [&](std::shared_ptr<AnimationTransition> & transition)
				{
					transition->SetOption(0.1f, false);
					transition->AddConditionFloat(walkValue, Less, 0.1f);
				});
				run->AddTransition(walk, [&](std::shared_ptr<AnimationTransition> & transition)
				{
					transition->SetOption(0.05f, false);
					transition->AddConditionFloat(walkValue, Less, 0.5f);
				});
				run->AddTransition(jump, [&](std::shared_ptr<AnimationTransition> & transition)
				{
					transition->SetOption(0.1f, false);
					transition->AddConditionTrigger(jumpTrigger);
				});
				run->AddTransition(fall, [&](std::shared_ptr<AnimationTransition> & transition)
				{
					transition->SetOption(0.2f, false);
					transition->AddConditionBool(isFall, true);
				});

				run->AddTransition(roll, [&](std::shared_ptr<AnimationTransition> & transition)
				{
					transition->SetOption(0.05f, false);
					transition->AddConditionTrigger(rollTrigger);
				});
			}
			// Roll
			{
				roll->AddTransition(rollStop, [&](std::shared_ptr<AnimationTransition> & transition)
				{
					transition->SetOption(0.05f, false);
					transition->AddConditionTrigger(rollStopTrigger);
				});

				roll->AddTransition(idle, [&](std::shared_ptr<AnimationTransition> & transition)
				{
					transition->SetOption(0.05f, true);
					transition->AddConditionFloat(walkValue, Less, 0.1f);
				});
				roll->AddTransition(walk, [&](std::shared_ptr<AnimationTransition> & transition)
				{
					transition->SetOption(0.1f, true);
					transition->AddConditionFloat(walkValue, Less, 0.5f);
				});
				roll->AddTransition(run, [&](std::shared_ptr<AnimationTransition> & transition)
				{
					transition->SetOption(0.1f, true);
					transition->AddConditionFloat(walkValue, Greater, 0.5f);
				});
				roll->AddTransition(jump, [&](std::shared_ptr<AnimationTransition> & transition)
				{
					transition->SetOption(0.05f, false);
					transition->AddConditionTrigger(jumpTrigger);
				});
				roll->AddTransition(fall, [&](std::shared_ptr<AnimationTransition> & transition)
				{
					transition->SetOption(0.1f, false);
					transition->AddConditionBool(isFall, true);
				});
			}
			// RollStop
			{
				rollStop->AddTransition(idle, [&](std::shared_ptr<AnimationTransition> & transition)
				{
					transition->SetOption(0.0f, true);
				});
			}
			// Jump
			{
				jump->AddTransition(cliff_Grap, [&](std::shared_ptr<AnimationTransition> & transition)
				{
					transition->SetOption(0.0f, false);
					transition->AddConditionBool(isCliff_Grap, true);
				});
				jump->AddTransition(run, [&](std::shared_ptr<AnimationTransition> & transition)
				{
					transition->SetOption(0.1f, false);
					transition->AddConditionBool(isFall, false);
					transition->AddConditionFloat(walkValue, Greater, 0.5f);
				});
				jump->AddTransition(walk, [&](std::shared_ptr<AnimationTransition> & transition)
				{
					transition->SetOption(0.1f, false);
					transition->AddConditionBool(isFall, false);
					transition->AddConditionFloat(walkValue, Greater, 0.1f);
				});
				jump->AddTransition(land, [&](std::shared_ptr<AnimationTransition> & transition)
				{
					transition->SetOption(0.03f, false);
					transition->AddConditionBool(isFall, false);
				});
			}
			// Fall
			{
				fall->AddTransition(cliff_Grap, [&](std::shared_ptr<AnimationTransition> & transition)
				{
					transition->SetOption(0.0f, false);
					transition->AddConditionBool(isCliff_Grap, true);
				});
				fall->AddTransition(run, [&](std::shared_ptr<AnimationTransition> & transition)
				{
					transition->SetOption(0.1f, false);
					transition->AddConditionBool(isFall, false);
					transition->AddConditionFloat(walkValue, Greater, 0.5f);
				});
				fall->AddTransition(walk, [&](std::shared_ptr<AnimationTransition> & transition)
				{
					transition->SetOption(0.1f, false);
					transition->AddConditionBool(isFall, false);
					transition->AddConditionFloat(walkValue, Greater, 0.1f);
				});
				fall->AddTransition(land, [&](std::shared_ptr<AnimationTransition> & transition)
				{
					transition->SetOption(0.03f, false);
					transition->AddConditionBool(isFall, false);
				});
			}
			// Land
			{
				land->AddTransition(idle, [&](std::shared_ptr<AnimationTransition> & transition)
				{
					transition->SetOption(0.0f, true);
				});
				land->AddTransition(run, [&](std::shared_ptr<AnimationTransition> & transition)
				{
					transition->SetOption(0.2f, false);
					transition->AddConditionFloat(walkValue, Greater, 0.5f);
				});
				land->AddTransition(walk, [&](std::shared_ptr<AnimationTransition> & transition)
				{
					transition->SetOption(0.2f, false);
					transition->AddConditionFloat(walkValue, Greater, 0.1f);
				});
			}
			// Cliff_Jump
			{
				cliff_Jump->AddTransition(cliff_Grap, [&](std::shared_ptr<AnimationTransition> & transition)
				{
					transition->SetOption(0.0f, false);
					transition->AddConditionBool(isCliff_Grap, true);
				});
				cliff_Jump->AddTransition(land, [&](std::shared_ptr<AnimationTransition> & transition)
				{
					transition->SetOption(0.1f, true, 0.5f);
					transition->AddConditionBool(isFall, false);
				});
			}
			// Cliff_Grap
			{
				cliff_Grap->AddTransition(cliff_Idle, [&](std::shared_ptr<AnimationTransition> & transition)
				{
					transition->SetOption(0.0f, true);
				});
				cliff_Grap->AddTransition(cliff_Up, [&](std::shared_ptr<AnimationTransition> & transition)
				{
					transition->SetOption(0.05f, false);
					transition->AddConditionTrigger(cliff_Up_Trigger);
				});
				cliff_Grap->AddTransition(fall, [&](std::shared_ptr<AnimationTransition> & transition)
				{
					transition->SetOption(0.1f, false);
					transition->AddConditionBool(isCliff_Grap, false);
				});
			}
			// Cliff_Idle
			{
				cliff_Idle->AddTransition(cliff_Up, [&](std::shared_ptr<AnimationTransition> & transition)
				{
					transition->SetOption(0.0f, false);
					transition->AddConditionTrigger(cliff_Up_Trigger);
				});
				cliff_Idle->AddTransition(fall, [&](std::shared_ptr<AnimationTransition> & transition)
				{
					transition->SetOption(0.1f, false);
					transition->AddConditionBool(isCliff_Grap, false);
				});
			}
			// Cliff_Up
			{
				cliff_Up->AddTransition(idle, [&](std::shared_ptr<AnimationTransition> & transition)
				{
					transition->SetOption(0.0f, true);
				});
			}
		});
		
	}
};

CEREAL_CLASS_VERSION(RabitAnimationController, 0)
CEREAL_REGISTER_TYPE(RabitAnimationController)

#endif // !_RABITANIMATIONCONTROLLER_H_

