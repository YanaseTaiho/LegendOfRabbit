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

	// 武器切り替え
	auto weapon_Change_Trigger = AddParameterTrigger("Weapon_Change_Trigger");

	// 攻撃系
	auto attack_Combo = AddParameterInt("Attack_Combo");
	auto attack_Type = AddParameterInt("Attack_Type");
	auto attack_Trigger = AddParameterTrigger("Attack_Trigger");
	auto attack_Jump_Trigger = AddParameterTrigger("Attack_Jump_Trigger");
	auto attack_Jump_Land_Trigger = AddParameterTrigger("Attack_Jump_Land_Trigger");

	// ロックオン系
	auto isRockOn = AddParameterBool("IsRockOn", false);
	auto RockOnDirection = AddParameterInt("RockOnDirection");
	auto step_Trigger = AddParameterTrigger("Step_Trigger");
	auto step_Trigger_Land = AddParameterTrigger("Step_Trigger_Land");

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

		// 武器付け替え
		auto weapon_Change = filter->AddState("Weapon_Change");

		// ロックオン時のフィルター
		auto RockOn_Filter = filter->AddFilter("RockOn_Filter");
		auto rockOn_Idle = RockOn_Filter->AddState("RockOn_Idle");
		// ロックオン左移動
		auto RockOn_Left_Filter = RockOn_Filter->AddFilter("RockOn_Left_Filter");
		auto rockOn_Left_Walk = RockOn_Left_Filter->AddState("RockOn_Left_Walk");
		auto rockOn_Left_Run = RockOn_Left_Filter->AddState("RockOn_Left_Run");
		// ロックオン右移動
		auto RockOn_Right_Filter = RockOn_Filter->AddFilter("RockOn_Right_Filter");
		auto rockOn_Right_Walk = RockOn_Right_Filter->AddState("RockOn_Right_Walk");
		auto rockOn_Right_Run = RockOn_Right_Filter->AddState("RockOn_Right_Run");
		// ロックオン後ろ移動
		auto RockOn_Back_Filter = RockOn_Filter->AddFilter("RockOn_Back_Filter");
		auto rockOn_Back_Walk = RockOn_Back_Filter->AddState("RockOn_Back_Walk");
		auto rockOn_Back_Run = RockOn_Back_Filter->AddState("RockOn_Back_Run");

		// ステップ移動
		auto rockOn_Left_Step = filter->AddState("RockOn_Left_Step");
		auto rockOn_Right_Step = filter->AddState("RockOn_Right_Step");
		auto rockOn_Back_Step = filter->AddState("RockOn_Back_Step");

		// ジャンプ切り
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


		// 攻撃モーションへ
		auto AttackTransition = [=](std::shared_ptr<AnimationFilter> filter, std::shared_ptr<AnimationState> state, PlayerActor::AttackType type, int combo)
		{
			filter->AddTransition(state, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.0f, false);
				transition->AddConditionInt(attack_Type, (int)type);
				transition->AddConditionInt(attack_Combo, combo);
				transition->AddConditionTrigger(attack_Trigger);
			});
		};
		auto StateAttackTransition = [=](std::shared_ptr<AnimationState> entry, std::shared_ptr<AnimationState> state, PlayerActor::AttackType type, int combo)
		{
			entry->AddTransition(state, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.0f, false);
				transition->AddConditionInt(attack_Type, (int)type);
				transition->AddConditionInt(attack_Combo, combo);
				transition->AddConditionTrigger(attack_Trigger);
			});
		};
		
		// RockOn_Filter
		{
			RockOn_Filter->AddTransition(Idle_Filter, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.1f, false);
				transition->AddConditionBool(isRockOn, false);
			});
			RockOn_Filter->AddTransition(attack_Jump, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.0f, false);
				transition->AddConditionTrigger(attack_Jump_Trigger);
			});
			RockOn_Filter->AddTransition(Move_Filter, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.1f, false);
				transition->AddConditionBool(isRockOn, true);
				transition->AddConditionFloat(walkValue, Greater, 0.2f);
				transition->AddConditionInt(RockOnDirection, (int)PlayerActor::Direction::Forward);
			});

			AttackTransition(RockOn_Filter, attack_Inside_1, PlayerActor::AttackType::Inside, 0);
			AttackTransition(RockOn_Filter, attack_Outside_1, PlayerActor::AttackType::Outside, 0);
			AttackTransition(RockOn_Filter, attack_Upper_1, PlayerActor::AttackType::Upper, 0);
			AttackTransition(RockOn_Filter, attack_Thrust_1, PlayerActor::AttackType::Thrust, 0);

			AttackTransition(RockOn_Filter, attack_Inside_2, PlayerActor::AttackType::Inside, 1);
			AttackTransition(RockOn_Filter, attack_Outside_2, PlayerActor::AttackType::Outside, 1);
			AttackTransition(RockOn_Filter, attack_Upper_2, PlayerActor::AttackType::Upper, 1);
			AttackTransition(RockOn_Filter, attack_Thrust_2, PlayerActor::AttackType::Thrust, 1);

			AttackTransition(RockOn_Filter, attack_Inside_3, PlayerActor::AttackType::Inside, 2);
			AttackTransition(RockOn_Filter, attack_Outside_3, PlayerActor::AttackType::Outside, 2);
			AttackTransition(RockOn_Filter, attack_Upper_3, PlayerActor::AttackType::Upper, 2);
			AttackTransition(RockOn_Filter, attack_Thrust_3, PlayerActor::AttackType::Thrust, 2);

			// rockOn_Idle
			rockOn_Idle->AddTransition(rockOn_Left_Walk, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.1f, false);
				transition->AddConditionFloat(walkValue, Greater, 0.1f);
				transition->AddConditionInt(RockOnDirection, (int)PlayerActor::Direction::Left);
			});
			rockOn_Idle->AddTransition(rockOn_Right_Walk, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.1f, false);
				transition->AddConditionFloat(walkValue, Greater, 0.1f);
				transition->AddConditionInt(RockOnDirection, (int)PlayerActor::Direction::Right);
			});
			rockOn_Idle->AddTransition(rockOn_Back_Walk, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.1f, false);
				transition->AddConditionFloat(walkValue, Greater, 0.1f);
				transition->AddConditionInt(RockOnDirection, (int)PlayerActor::Direction::Back);
			});

			filter->AddTransition(rockOn_Left_Step, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.1f, false);
				transition->AddConditionTrigger(step_Trigger);
				transition->AddConditionBool(isRockOn, true);
				transition->AddConditionInt(RockOnDirection, (int)PlayerActor::Direction::Left);
			});
			filter->AddTransition(rockOn_Right_Step, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.1f, false);
				transition->AddConditionTrigger(step_Trigger);
				transition->AddConditionBool(isRockOn, true);
				transition->AddConditionInt(RockOnDirection, (int)PlayerActor::Direction::Right);
			});
			filter->AddTransition(rockOn_Back_Step, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.1f, false);
				transition->AddConditionTrigger(step_Trigger);
				transition->AddConditionBool(isRockOn, true);
				transition->AddConditionInt(RockOnDirection, (int)PlayerActor::Direction::Back);
			});
			filter->AddTransition(weapon_Change, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.1f, false);
				transition->AddConditionTrigger(weapon_Change_Trigger);
			});

			// RockOn_Left_Filter
			RockOn_Left_Filter->AddTransition(rockOn_Idle, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.1f, false);
				transition->AddConditionFloat(walkValue, Less, 0.1f);
			});
			RockOn_Left_Filter->AddTransition(rockOn_Right_Walk, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.1f, false);
				transition->AddConditionFloat(walkValue, Greater, 0.1f);
				transition->AddConditionInt(RockOnDirection, (int)PlayerActor::Direction::Right);
			});
			RockOn_Left_Filter->AddTransition(rockOn_Back_Walk, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.1f, false);
				transition->AddConditionFloat(walkValue, Greater, 0.1f);
				transition->AddConditionInt(RockOnDirection, (int)PlayerActor::Direction::Back);
			});	


			// RockOn_Right_Filter
			RockOn_Right_Filter->AddTransition(rockOn_Idle, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.1f, false);
				transition->AddConditionFloat(walkValue, Less, 0.1f);
			});
			RockOn_Right_Filter->AddTransition(rockOn_Left_Walk, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.1f, false);
				transition->AddConditionFloat(walkValue, Greater, 0.1f);
				transition->AddConditionInt(RockOnDirection, (int)PlayerActor::Direction::Left);
			});
			RockOn_Right_Filter->AddTransition(rockOn_Back_Walk, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.1f, false);
				transition->AddConditionFloat(walkValue, Greater, 0.1f);
				transition->AddConditionInt(RockOnDirection, (int)PlayerActor::Direction::Back);
			});
			// RockOn_Back_Filter
			RockOn_Back_Filter->AddTransition(rockOn_Idle, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.1f, false);
				transition->AddConditionFloat(walkValue, Less, 0.1f);
			});
			RockOn_Back_Filter->AddTransition(rockOn_Left_Walk, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.1f, false);
				transition->AddConditionFloat(walkValue, Greater, 0.1f);
				transition->AddConditionInt(RockOnDirection, (int)PlayerActor::Direction::Left);
			});
			RockOn_Back_Filter->AddTransition(rockOn_Right_Walk, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.1f, false);
				transition->AddConditionFloat(walkValue, Greater, 0.1f);
				transition->AddConditionInt(RockOnDirection, (int)PlayerActor::Direction::Right);
			});

			rockOn_Left_Walk->AddTransition(rockOn_Left_Run, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.1f, false);
				transition->AddConditionFloat(walkValue, Greater, 0.5f);
				transition->AddConditionInt(RockOnDirection, (int)PlayerActor::Direction::Left);
			});
			// 左ステップ
			
			rockOn_Left_Step->AddTransition(attack_Jump, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.1f, false);
				transition->AddConditionTrigger(attack_Jump_Trigger);
			});
			/*rockOn_Left_Step->AddTransition(rockOn_Left_Run, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.1f, false);
				transition->AddConditionTrigger(step_Trigger_Land);
				transition->AddConditionBool(isRockOn, true);
				transition->AddConditionFloat(walkValue, Greater, 0.5f);
			});*/
			rockOn_Left_Step->AddTransition(rockOn_Idle, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.1f, false);
				transition->AddConditionTrigger(step_Trigger_Land);
				transition->AddConditionBool(isRockOn, true);
			});
			rockOn_Left_Step->AddTransition(idle, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.1f, false);
				transition->AddConditionBool(isRockOn, false);
				transition->AddConditionTrigger(step_Trigger_Land);
			});

			rockOn_Right_Walk->AddTransition(rockOn_Right_Run, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.1f, false);
				transition->AddConditionFloat(walkValue, Greater, 0.5f);
				transition->AddConditionInt(RockOnDirection, (int)PlayerActor::Direction::Right);
			});

			// 右ステップ
			
			rockOn_Right_Step->AddTransition(attack_Jump, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.1f, false);
				transition->AddConditionTrigger(attack_Jump_Trigger);
			});
			/*rockOn_Right_Step->AddTransition(rockOn_Right_Run, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.1f, false);
				transition->AddConditionTrigger(step_Trigger_Land);
				transition->AddConditionBool(isRockOn, true);
				transition->AddConditionFloat(walkValue, Greater, 0.5f);
			});*/
			rockOn_Right_Step->AddTransition(rockOn_Idle, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.1f, false);
				transition->AddConditionTrigger(step_Trigger_Land);
				transition->AddConditionBool(isRockOn, true);
			});
			rockOn_Right_Step->AddTransition(idle, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.1f, false);
				transition->AddConditionTrigger(step_Trigger_Land);
				transition->AddConditionBool(isRockOn, false);
			});

			rockOn_Back_Walk->AddTransition(rockOn_Back_Run, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.1f, false);
				transition->AddConditionFloat(walkValue, Greater, 0.5f);
				transition->AddConditionInt(RockOnDirection, (int)PlayerActor::Direction::Back);
			});

			// バックステップ
			
			/*rockOn_Back_Step->AddTransition(attack_Jump, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.1f, false);
				transition->AddConditionTrigger(attack_Jump_Trigger);
			});*/
			/*rockOn_Back_Step->AddTransition(rockOn_Back_Run, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.1f, false);
				transition->AddConditionTrigger(step_Trigger_Land);
				transition->AddConditionBool(isRockOn, true);
				transition->AddConditionFloat(walkValue, Greater, 0.5f);
			});*/
			rockOn_Back_Step->AddTransition(rockOn_Idle, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.1f, false);
				transition->AddConditionTrigger(step_Trigger_Land);
				transition->AddConditionBool(isRockOn, true);
			});
			rockOn_Back_Step->AddTransition(idle, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.1f, false);
				transition->AddConditionTrigger(step_Trigger_Land);
				transition->AddConditionBool(isRockOn, false);
			});
		}

		// Attack_Jump
		{
			attack_Jump->AddTransition(attack_Jump_Land, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.0f, false);
				transition->AddConditionBool(isFall, false);
				transition->AddConditionTrigger(attack_Jump_Land_Trigger);
			});
		}
		// Attack_Jump_Land
		{
			attack_Jump_Land->AddTransition(idle, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.1f, true);
			});
			attack_Jump_Land->AddTransition(attack_Jump, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.1f, false);
				transition->AddConditionBool(isFall, false);
				transition->AddConditionTrigger(attack_Jump_Trigger);
			});
		}
		// Weapon_Change
		{
			weapon_Change->AddTransition(idle, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.05f, true);
			});
			weapon_Change->AddTransition(attack_Jump, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.05f, false);
				transition->AddConditionTrigger(attack_Jump_Trigger);
			});

			StateAttackTransition(weapon_Change, attack_Inside_1, PlayerActor::AttackType::Inside, 0);
			StateAttackTransition(weapon_Change, attack_Outside_1, PlayerActor::AttackType::Outside, 0);
			StateAttackTransition(weapon_Change, attack_Upper_1, PlayerActor::AttackType::Upper, 0);
			StateAttackTransition(weapon_Change, attack_Thrust_1, PlayerActor::AttackType::Thrust, 0);

			StateAttackTransition(weapon_Change, attack_Inside_2, PlayerActor::AttackType::Inside, 1);
			StateAttackTransition(weapon_Change, attack_Outside_2, PlayerActor::AttackType::Outside, 1);
			StateAttackTransition(weapon_Change, attack_Upper_2, PlayerActor::AttackType::Upper, 1);
			StateAttackTransition(weapon_Change, attack_Thrust_2, PlayerActor::AttackType::Thrust, 1);

			StateAttackTransition(weapon_Change, attack_Inside_3, PlayerActor::AttackType::Inside, 2);
			StateAttackTransition(weapon_Change, attack_Outside_3, PlayerActor::AttackType::Outside, 2);
			StateAttackTransition(weapon_Change, attack_Upper_3, PlayerActor::AttackType::Upper, 2);
			StateAttackTransition(weapon_Change, attack_Thrust_3, PlayerActor::AttackType::Thrust, 2);
		}

		// Attack_Filter
		{
			Attack_Filter->AddTransition(rockOn_Idle, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.2f, true, 0.7f);
				transition->AddConditionFloat(walkValue, Less, 0.1f);
				transition->AddConditionBool(isRockOn, true);
			});
			Attack_Filter->AddTransition(Idle_Filter, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.2f, true, 0.7f);
				transition->AddConditionFloat(walkValue, Less, 0.1f);
			});
			Attack_Filter->AddTransition(run, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.2f, true, 0.8f);
				transition->AddConditionFloat(walkValue, Greater, 0.5f);
			});
			Attack_Filter->AddTransition(walk, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.2f, true, 0.8f);
				transition->AddConditionFloat(walkValue, Greater, 0.1f);
			});
			Attack_Filter->AddTransition(fall, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.05f, false);
				transition->AddConditionBool(isFall, true);
			});
		}

		// エントリーポイント設定
		filter->SetEntryPoint(idle);

		//======================= トランジション追加 =======================//

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
			Idle_Filter->AddTransition(attack_Jump, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.03f, false);
				transition->AddConditionTrigger(attack_Jump_Trigger);
			});
			Idle_Filter->AddTransition(RockOn_Filter, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.1f, false);
				transition->AddConditionTrigger(isRockOn);
			});
			
			AttackTransition(Idle_Filter, attack_Inside_1, PlayerActor::AttackType::Inside, 0);
			AttackTransition(Idle_Filter, attack_Outside_1, PlayerActor::AttackType::Outside, 0);
			AttackTransition(Idle_Filter, attack_Upper_1, PlayerActor::AttackType::Upper, 0);
			AttackTransition(Idle_Filter, attack_Thrust_1, PlayerActor::AttackType::Thrust, 0);

			AttackTransition(Idle_Filter, attack_Inside_2, PlayerActor::AttackType::Inside, 1);
			AttackTransition(Idle_Filter, attack_Outside_2, PlayerActor::AttackType::Outside, 1);
			AttackTransition(Idle_Filter, attack_Upper_2, PlayerActor::AttackType::Upper, 1);
			AttackTransition(Idle_Filter, attack_Thrust_2, PlayerActor::AttackType::Thrust, 1);

			AttackTransition(Idle_Filter, attack_Inside_3, PlayerActor::AttackType::Inside, 2);
			AttackTransition(Idle_Filter, attack_Outside_3, PlayerActor::AttackType::Outside, 2);
			AttackTransition(Idle_Filter, attack_Upper_3, PlayerActor::AttackType::Upper, 2);
			AttackTransition(Idle_Filter, attack_Thrust_3, PlayerActor::AttackType::Thrust, 2);
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
			Move_Filter->AddTransition(attack_Jump, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.03f, false);
				transition->AddConditionTrigger(attack_Jump_Trigger);
			});
			Move_Filter->AddTransition(rockOn_Idle, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.1f, false);
				transition->AddConditionTrigger(isRockOn);
				transition->AddConditionFloat(walkValue, Less, 0.1f);
			});
			Move_Filter->AddTransition(rockOn_Left_Walk, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.1f, false);
				transition->AddConditionTrigger(isRockOn);
				transition->AddConditionInt(RockOnDirection, (int)PlayerActor::Direction::Left);
			});
			Move_Filter->AddTransition(rockOn_Right_Walk, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.1f, false);
				transition->AddConditionTrigger(isRockOn);
				transition->AddConditionInt(RockOnDirection, (int)PlayerActor::Direction::Right);
			});
			Move_Filter->AddTransition(rockOn_Back_Walk, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.1f, false);
				transition->AddConditionTrigger(isRockOn);
				transition->AddConditionInt(RockOnDirection, (int)PlayerActor::Direction::Back);
			});

			// 攻撃モーションへ
			AttackTransition(Move_Filter, attack_Inside_1, PlayerActor::AttackType::Inside, 0);
			AttackTransition(Move_Filter, attack_Outside_1, PlayerActor::AttackType::Outside, 0);
			AttackTransition(Move_Filter, attack_Upper_1, PlayerActor::AttackType::Upper, 0);
			AttackTransition(Move_Filter, attack_Thrust_1, PlayerActor::AttackType::Thrust, 0);

			AttackTransition(Move_Filter, attack_Inside_2, PlayerActor::AttackType::Inside, 1);
			AttackTransition(Move_Filter, attack_Outside_2, PlayerActor::AttackType::Outside, 1);
			AttackTransition(Move_Filter, attack_Upper_2, PlayerActor::AttackType::Upper, 1);
			AttackTransition(Move_Filter, attack_Thrust_2, PlayerActor::AttackType::Thrust, 1);

			AttackTransition(Move_Filter, attack_Inside_3, PlayerActor::AttackType::Inside, 2);
			AttackTransition(Move_Filter, attack_Outside_3, PlayerActor::AttackType::Outside, 2);
			AttackTransition(Move_Filter, attack_Upper_3, PlayerActor::AttackType::Upper, 2);
			AttackTransition(Move_Filter, attack_Thrust_3, PlayerActor::AttackType::Thrust, 2);

			Move_Filter->AddTransition(cliff_Jump, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.03f, false);
				transition->AddConditionTrigger(cliff_Jump_Trigger);
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
				transition->SetOption(0.3f, true, 0.95f);
			});
		}
		// Jump
		{
			jump->AddTransition(cliff_Grap, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.0f, false);
				transition->AddConditionBool(isCliff_Grap, true);
			});
			jump->AddTransition(attack_Jump, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.0f, false);
				transition->AddConditionTrigger(attack_Jump_Trigger);
			});
			jump->AddTransition(run, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.1f, false);
				transition->AddConditionBool(isFall, false);
				transition->AddConditionFloat(walkValue, Greater, 0.7f);
			});
			jump->AddTransition(walk, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.1f, false);
				transition->AddConditionBool(isFall, false);
				transition->AddConditionFloat(walkValue, Greater, 0.3f);
			});
			jump->AddTransition(land, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.01f, false);
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
			fall->AddTransition(attack_Jump, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.0f, false);
				transition->AddConditionTrigger(attack_Jump_Trigger);
			});
			fall->AddTransition(run, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.1f, false);
				transition->AddConditionBool(isFall, false);
				transition->AddConditionFloat(walkValue, Greater, 0.7f);
			});
			fall->AddTransition(walk, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.1f, false);
				transition->AddConditionBool(isFall, false);
				transition->AddConditionFloat(walkValue, Greater, 0.3f);
			});
			fall->AddTransition(land, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.01f, false);
				transition->AddConditionBool(isFall, false);
			});
		}
		// Land
		{
			land->AddTransition(idle, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.1f, true, 0.90f);
			});
			land->AddTransition(run, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.3f, false);
				transition->AddConditionFloat(walkValue, Greater, 0.8f);
			});
			land->AddTransition(walk, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.2f, false);
				transition->AddConditionFloat(walkValue, Greater, 0.4f);
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
