#include "RabitAnimationController.h"
#include "../Actor/Player/PlayerActor.h"

void RabitAnimationController::Initialize()
{
	AnimationController::Initialize();

	// パラメータ追加
	auto walkValue = AddParameterFloat("WalkValue", 0.0f);
	auto jumpTrigger = AddParameterTrigger("JumpTrigger", false);
	auto rollTrigger = AddParameterTrigger("RollTrigger", false);
	auto rollStopTrigger = AddParameterTrigger("RollStopTrigger", false);
	auto isFall = AddParameterBool("IsFall", false);

	// 崖掴み系
	auto cliff_Jump_Trigger = AddParameterTrigger("Cliff_Jump_Trigger", false);
	auto isCliff_Grap = AddParameterBool("IsCliff_Grap", false);
	auto cliff_Up_Trigger = AddParameterTrigger("Cliff_Up_Trigger", false);

	// 攻撃系
	auto attack_Combo = AddParameterInt("Attack_Combo");
	auto attack_Type = AddParameterInt("Attack_Type");
	auto attack_Trigger = AddParameterTrigger("Attack_Trigger");

	AddFilter("Normal", [=](std::shared_ptr<AnimationFilter> & filter)
	{
		// ステート追加
		auto Idle_Filter = filter->AddFilter("Idle_Filter");
		auto idle = Idle_Filter->AddState("Idle");

		auto Move_Filter = filter->AddFilter("Move_Filter");
		auto walk = Move_Filter->AddState("Walk");
		auto run = Move_Filter->AddState("Run");

		auto roll = filter->AddState("Roll");
		auto rollStop = filter->AddState("RollStop");
		auto jump = filter->AddState("Jump");
		auto fall = filter->AddState("Fall");
		auto land = filter->AddState("Land");

		auto cliff_Jump = filter->AddState("Cliff_Jump");
		auto cliff_Grap = filter->AddState("Cliff_Grap");
		auto cliff_Idle = filter->AddState("Cliff_Idle");
		auto cliff_Up = filter->AddState("Cliff_Up");

		auto attack_Jump = filter->AddState("Attack_Jump");
		auto attack_Jump_Land = filter->AddState("Attack_Jump_Land");

		// 通常攻撃用のフィルター
		auto Attack_Filter = filter->AddFilter("Attack_Filter");

		auto Attack_Inside_Filter = Attack_Filter->AddFilter("Attack_Inside_Filter");
		auto attack_Inside_1 = Attack_Inside_Filter->AddState("Attack_Inside_1");
		auto attack_Inside_2 = Attack_Inside_Filter->AddState("Attack_Inside_2");
		auto attack_Inside_3 = Attack_Inside_Filter->AddState("Attack_Inside_3");

		auto Attack_Outside_Filter = Attack_Filter->AddFilter("Attack_Outside_Filter");
		auto attack_Outside_1 = Attack_Outside_Filter->AddState("Attack_Outside_1");
		auto attack_Outside_2 = Attack_Outside_Filter->AddState("Attack_Outside_2");
		auto attack_Outside_3 = Attack_Outside_Filter->AddState("Attack_Outside_3");

		auto Attack_Upper_Filter = Attack_Filter->AddFilter("Attack_Upper_Filter");
		auto attack_Upper_1 = Attack_Upper_Filter->AddState("Attack_Upper_1");
		auto attack_Upper_2 = Attack_Upper_Filter->AddState("Attack_Upper_2");
		auto attack_Upper_3 = Attack_Upper_Filter->AddState("Attack_Upper_3");

		auto Attack_Thrust_Filter = Attack_Filter->AddFilter("Attack_Thrust_Filter");
		auto attack_Thrust_1 = Attack_Thrust_Filter->AddState("Attack_Thrust_1");
		auto attack_Thrust_2 = Attack_Thrust_Filter->AddState("Attack_Thrust_2");
		auto attack_Thrust_3 = Attack_Thrust_Filter->AddState("Attack_Thrust_3");

		// Attack_Filter
		{
			Attack_Filter->AddTransition(Idle_Filter, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.1f, true, 0.9f);
				transition->AddConditionFloat(walkValue, Less, 0.1f);
			});
			Attack_Filter->AddTransition(Move_Filter, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.1f, true, 0.9f);
				transition->AddConditionFloat(walkValue, Greater, 0.1f);
			});
			Attack_Filter->AddTransition(fall, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.05f, false);
				transition->AddConditionBool(isFall, true);
			});

			// Inside
			{
				Attack_Inside_Filter->AddTransition(attack_Outside_1, [=](std::shared_ptr<AnimationTransition> & transition)
				{
					transition->SetOption(0.0f, true, 0.7f);
					transition->AddConditionInt(attack_Type, (int)PlayerActor::AttackType::Outside);
					transition->AddConditionInt(attack_Combo, 0);
				});
				Attack_Inside_Filter->AddTransition(attack_Upper_1, [=](std::shared_ptr<AnimationTransition> & transition)
				{
					transition->SetOption(0.0f, true, 0.7f);
					transition->AddConditionInt(attack_Type, (int)PlayerActor::AttackType::Upper);
					transition->AddConditionInt(attack_Combo, 0);
				});
				Attack_Inside_Filter->AddTransition(attack_Thrust_1, [=](std::shared_ptr<AnimationTransition> & transition)
				{
					transition->SetOption(0.0f, true, 0.7f);
					transition->AddConditionInt(attack_Type, (int)PlayerActor::AttackType::Thrust);
					transition->AddConditionInt(attack_Combo, 0);
				});

				attack_Inside_1->AddTransition(attack_Inside_2, [=](std::shared_ptr<AnimationTransition> & transition)
				{
					transition->SetOption(0.0f, true, 0.7f);
					transition->AddConditionInt(attack_Type, (int)PlayerActor::AttackType::Inside);
					transition->AddConditionInt(attack_Combo, 1);
				});
				attack_Inside_2->AddTransition(attack_Inside_3, [=](std::shared_ptr<AnimationTransition> & transition)
				{
					transition->SetOption(0.0f, true, 0.7f);
					transition->AddConditionInt(attack_Type, (int)PlayerActor::AttackType::Inside);
					transition->AddConditionInt(attack_Combo, 2);
				});
			}
			// Outside
			{
				Attack_Outside_Filter->AddTransition(attack_Inside_1, [=](std::shared_ptr<AnimationTransition> & transition)
				{
					transition->SetOption(0.0f, false);
					transition->AddConditionInt(attack_Type, (int)PlayerActor::AttackType::Inside);
					transition->AddConditionInt(attack_Combo, 0);
				});
				Attack_Outside_Filter->AddTransition(attack_Upper_1, [=](std::shared_ptr<AnimationTransition> & transition)
				{
					transition->SetOption(0.0f, false);
					transition->AddConditionInt(attack_Type, (int)PlayerActor::AttackType::Upper);
					transition->AddConditionInt(attack_Combo, 0);
				});
				Attack_Outside_Filter->AddTransition(attack_Thrust_1, [=](std::shared_ptr<AnimationTransition> & transition)
				{
					transition->SetOption(0.0f, false);
					transition->AddConditionInt(attack_Type, (int)PlayerActor::AttackType::Thrust);
					transition->AddConditionInt(attack_Combo, 0);
				});

				attack_Outside_1->AddTransition(attack_Outside_2, [=](std::shared_ptr<AnimationTransition> & transition)
				{
					transition->SetOption(0.0f, false);
					transition->AddConditionInt(attack_Type, (int)PlayerActor::AttackType::Outside);
					transition->AddConditionInt(attack_Combo, 1);
				});
				attack_Outside_2->AddTransition(attack_Outside_3, [=](std::shared_ptr<AnimationTransition> & transition)
				{
					transition->SetOption(0.0f, false);
					transition->AddConditionInt(attack_Type, (int)PlayerActor::AttackType::Outside);
					transition->AddConditionInt(attack_Combo, 2);
				});
			}
			// Upper
			{
				Attack_Upper_Filter->AddTransition(attack_Inside_1, [=](std::shared_ptr<AnimationTransition> & transition)
				{
					transition->SetOption(0.0f, false);
					transition->AddConditionInt(attack_Type, (int)PlayerActor::AttackType::Inside);
					transition->AddConditionInt(attack_Combo, 0);
				});
				Attack_Upper_Filter->AddTransition(attack_Outside_1, [=](std::shared_ptr<AnimationTransition> & transition)
				{
					transition->SetOption(0.0f, false);
					transition->AddConditionInt(attack_Type, (int)PlayerActor::AttackType::Outside);
					transition->AddConditionInt(attack_Combo, 0);
				});
				Attack_Upper_Filter->AddTransition(attack_Thrust_1, [=](std::shared_ptr<AnimationTransition> & transition)
				{
					transition->SetOption(0.0f, false);
					transition->AddConditionInt(attack_Type, (int)PlayerActor::AttackType::Thrust);
					transition->AddConditionInt(attack_Combo, 0);
				});

				attack_Upper_1->AddTransition(attack_Upper_2, [=](std::shared_ptr<AnimationTransition> & transition)
				{
					transition->SetOption(0.0f, false);
					transition->AddConditionInt(attack_Type, (int)PlayerActor::AttackType::Upper);
					transition->AddConditionInt(attack_Combo, 1);
				});
				attack_Upper_2->AddTransition(attack_Upper_3, [=](std::shared_ptr<AnimationTransition> & transition)
				{
					transition->SetOption(0.0f, false);
					transition->AddConditionInt(attack_Type, (int)PlayerActor::AttackType::Upper);
					transition->AddConditionInt(attack_Combo, 2);
				});
			}
			// Thrust
			{
				Attack_Thrust_Filter->AddTransition(attack_Inside_1, [=](std::shared_ptr<AnimationTransition> & transition)
				{
					transition->SetOption(0.0f, false);
					transition->AddConditionInt(attack_Type, (int)PlayerActor::AttackType::Inside);
					transition->AddConditionInt(attack_Combo, 0);
				});
				Attack_Thrust_Filter->AddTransition(attack_Outside_1, [=](std::shared_ptr<AnimationTransition> & transition)
				{
					transition->SetOption(0.0f, false);
					transition->AddConditionInt(attack_Type, (int)PlayerActor::AttackType::Outside);
					transition->AddConditionInt(attack_Combo, 0);
				});
				Attack_Thrust_Filter->AddTransition(attack_Upper_1, [=](std::shared_ptr<AnimationTransition> & transition)
				{
					transition->SetOption(0.0f, false);
					transition->AddConditionInt(attack_Type, (int)PlayerActor::AttackType::Upper);
					transition->AddConditionInt(attack_Combo, 0);
				});

				attack_Thrust_1->AddTransition(attack_Thrust_2, [=](std::shared_ptr<AnimationTransition> & transition)
				{
					transition->SetOption(0.0f, false);
					transition->AddConditionInt(attack_Type, (int)PlayerActor::AttackType::Thrust);
					transition->AddConditionInt(attack_Combo, 1);
				});
				attack_Thrust_2->AddTransition(attack_Thrust_3, [=](std::shared_ptr<AnimationTransition> & transition)
				{
					transition->SetOption(0.0f, false);
					transition->AddConditionInt(attack_Type, (int)PlayerActor::AttackType::Thrust);
					transition->AddConditionInt(attack_Combo, 2);
				});
			}
		}


		// エントリーポイント設定
		filter->SetEntryPoint(idle);

		// トランジション追加
		//
		// Idle_Filter
		{
			Idle_Filter->AddTransition(walk, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.1f, false);
				transition->AddConditionFloat(walkValue, Greater, 0.1f);
			});
			Idle_Filter->AddTransition(jump, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.1f, false);
				transition->AddConditionTrigger(jumpTrigger);
			});
			Idle_Filter->AddTransition(fall, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.2f, false);
				transition->AddConditionBool(isFall, true);
			});
			Idle_Filter->AddTransition(cliff_Jump, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.03f, false);
				transition->AddConditionTrigger(cliff_Jump_Trigger);
			});
			Idle_Filter->AddTransition(Attack_Filter, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.0f, false);
				transition->AddConditionTrigger(attack_Trigger);
			});
		}

		// Move_Filter
		{
			Move_Filter->AddTransition(jump, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.1f, false);
				transition->AddConditionTrigger(jumpTrigger);
			});
			Move_Filter->AddTransition(fall, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.2f, false);
				transition->AddConditionBool(isFall, true);
			});
			Move_Filter->AddTransition(roll, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.05f, false);
				transition->AddConditionTrigger(rollTrigger);
			});
			Move_Filter->AddTransition(Attack_Filter, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.0f, false);
				transition->AddConditionTrigger(attack_Trigger);
			});

			// Walk
			{
				walk->AddTransition(idle, [=](std::shared_ptr<AnimationTransition> & transition)
				{
					transition->SetOption(0.2f, false);
					transition->AddConditionFloat(walkValue, Less, 0.1f);
				});
				walk->AddTransition(run, [=](std::shared_ptr<AnimationTransition> & transition)
				{
					transition->SetOption(0.3f, false);
					transition->AddConditionFloat(walkValue, Greater, 0.5f);
				});
				walk->AddTransition(cliff_Jump, [=](std::shared_ptr<AnimationTransition> & transition)
				{
					transition->SetOption(0.03f, false);
					transition->AddConditionTrigger(cliff_Jump_Trigger);
				});
			}
			// Run
			{
				run->AddTransition(idle, [=](std::shared_ptr<AnimationTransition> & transition)
				{
					transition->SetOption(0.1f, false);
					transition->AddConditionFloat(walkValue, Less, 0.1f);
				});
				run->AddTransition(walk, [=](std::shared_ptr<AnimationTransition> & transition)
				{
					transition->SetOption(0.05f, false);
					transition->AddConditionFloat(walkValue, Less, 0.5f);
				});
			}
		}

		// Roll
		{
			roll->AddTransition(rollStop, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.05f, false);
				transition->AddConditionTrigger(rollStopTrigger);
			});

			roll->AddTransition(idle, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.05f, true);
				transition->AddConditionFloat(walkValue, Less, 0.1f);
			});
			roll->AddTransition(walk, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.1f, true);
				transition->AddConditionFloat(walkValue, Less, 0.5f);
			});
			roll->AddTransition(run, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.1f, true);
				transition->AddConditionFloat(walkValue, Greater, 0.5f);
			});
			roll->AddTransition(jump, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.05f, false);
				transition->AddConditionTrigger(jumpTrigger);
			});
			roll->AddTransition(fall, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.1f, false);
				transition->AddConditionBool(isFall, true);
			});
		}
		// RollStop
		{
			rollStop->AddTransition(idle, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.0f, true);
			});
		}
		// Jump
		{
			jump->AddTransition(cliff_Grap, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.0f, false);
				transition->AddConditionBool(isCliff_Grap, true);
			});
			jump->AddTransition(run, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.1f, false);
				transition->AddConditionBool(isFall, false);
				transition->AddConditionFloat(walkValue, Greater, 0.5f);
			});
			jump->AddTransition(walk, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.1f, false);
				transition->AddConditionBool(isFall, false);
				transition->AddConditionFloat(walkValue, Greater, 0.1f);
			});
			jump->AddTransition(land, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.03f, false);
				transition->AddConditionBool(isFall, false);
			});
		}
		// Fall
		{
			fall->AddTransition(cliff_Grap, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.0f, false);
				transition->AddConditionBool(isCliff_Grap, true);
			});
			fall->AddTransition(run, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.1f, false);
				transition->AddConditionBool(isFall, false);
				transition->AddConditionFloat(walkValue, Greater, 0.5f);
			});
			fall->AddTransition(walk, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.1f, false);
				transition->AddConditionBool(isFall, false);
				transition->AddConditionFloat(walkValue, Greater, 0.1f);
			});
			fall->AddTransition(land, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.03f, false);
				transition->AddConditionBool(isFall, false);
			});
		}
		// Land
		{
			land->AddTransition(idle, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.0f, true);
			});
			land->AddTransition(run, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.2f, false);
				transition->AddConditionFloat(walkValue, Greater, 0.5f);
			});
			land->AddTransition(walk, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.2f, false);
				transition->AddConditionFloat(walkValue, Greater, 0.1f);
			});
		}
		// Cliff_Jump
		{
			cliff_Jump->AddTransition(cliff_Grap, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.0f, false);
				transition->AddConditionBool(isCliff_Grap, true);
			});
			cliff_Jump->AddTransition(land, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.1f, true, 0.5f);
				transition->AddConditionBool(isFall, false);
			});
		}
		// Cliff_Grap
		{
			cliff_Grap->AddTransition(cliff_Idle, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.0f, true);
			});
			cliff_Grap->AddTransition(cliff_Up, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.05f, false);
				transition->AddConditionTrigger(cliff_Up_Trigger);
			});
			cliff_Grap->AddTransition(fall, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.1f, false);
				transition->AddConditionBool(isCliff_Grap, false);
			});
		}
		// Cliff_Idle
		{
			cliff_Idle->AddTransition(cliff_Up, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.0f, false);
				transition->AddConditionTrigger(cliff_Up_Trigger);
			});
			cliff_Idle->AddTransition(fall, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.1f, false);
				transition->AddConditionBool(isCliff_Grap, false);
			});
		}
		// Cliff_Up
		{
			cliff_Up->AddTransition(idle, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.0f, true);
			});
		}
	});
}
