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
	auto attack_Flip_Trigger = AddParameterTrigger("Attack_Flip_Trigger");	// 攻撃を弾かれた時のトリガー

	// ロックオン系
	auto isRockOn = AddParameterBool("IsRockOn", false);
	auto RockOnDirection = AddParameterInt("RockOnDirection");
	auto step_Trigger = AddParameterTrigger("Step_Trigger");
	auto step_Trigger_Land = AddParameterTrigger("Step_Trigger_Land");

	// ダメージ系
	auto damage_Trigger = AddParameterTrigger("Damage_Trigger");
	auto damage_Land_Trigger = AddParameterTrigger("Damage_Land_Trigger");	// ダメージ時の着地トリガー
	auto damage_Direction = AddParameterInt("Damage_Direction");	// 吹き飛ぶダメージを受けた方向（０前方 １後方）

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

		// アタック弾かれ
		auto attack_Flip = filter->AddState("Attack_Flip");

		// ダメージ
		auto damage_Forward = filter->AddState("Damage_Forward");
		auto damage_Forward_Land = filter->AddState("Damage_Forward_Land");
		auto damage_Back = filter->AddState("Damage_Back");
		auto damage_Back_Land = filter->AddState("Damage_Back_Land");

		// エントリーポイント設定
		filter->SetEntryPoint(idle);

		//======================= トランジション追加 =======================//

		// ダメージは全ての状態から遷移可能
		filter->AddTransition(damage_Forward, [=](std::shared_ptr<AnimationTransition> & transition)
		{
			transition->SetOption(0.1f, false);
			transition->AddConditionTrigger(damage_Trigger);
			transition->AddConditionInt(damage_Direction, 0);	// 0の時前方
		});
		filter->AddTransition(damage_Back, [=](std::shared_ptr<AnimationTransition> & transition)
		{
			transition->SetOption(0.1f, false);
			transition->AddConditionTrigger(damage_Trigger);
			transition->AddConditionInt(damage_Direction, 1);	// 1の時前方
		});
		damage_Forward->AddTransition(damage_Forward_Land, [=](std::shared_ptr<AnimationTransition> & transition)
		{
			transition->SetOption(0.0f, false);
			transition->AddConditionTrigger(damage_Land_Trigger);
		});
		damage_Back->AddTransition(damage_Back_Land, [=](std::shared_ptr<AnimationTransition> & transition)
		{
			transition->SetOption(0.0f, false);
			transition->AddConditionTrigger(damage_Land_Trigger);
		});
		// ダメージ着地
		damage_Forward_Land->AddTransition(idle, [=](std::shared_ptr<AnimationTransition> & transition)
		{
			transition->SetOption(0.0f, true);
		});
		damage_Back_Land->AddTransition(idle, [=](std::shared_ptr<AnimationTransition> & transition)
		{
			transition->SetOption(0.0f, true);
		});

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
		
		filter->AddTransition(attack_Jump, [=](std::shared_ptr<AnimationTransition> & transition)
		{
			transition->SetOption(0.05f, false);
			transition->AddConditionTrigger(attack_Jump_Trigger);
		});

		// RockOn_Filter
		{
			RockOn_Filter->AddTransition(Idle_Filter, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.1f, false);
				transition->AddConditionBool(isRockOn, false);
			});
			RockOn_Filter->AddTransition(run, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.1f, false);
				transition->AddConditionBool(isRockOn, true);
				transition->AddConditionFloat(walkValue, Greater, 0.5f);
				transition->AddConditionInt(RockOnDirection, (int)PlayerActor::Direction::Forward);
			});
			RockOn_Filter->AddTransition(walk, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.1f, false);
				transition->AddConditionBool(isRockOn, true);
				transition->AddConditionFloat(walkValue, Greater, 0.2f);
				transition->AddConditionInt(RockOnDirection, (int)PlayerActor::Direction::Forward);
			});

			AttackTransition(filter, attack_Inside_1, PlayerActor::AttackType::Inside, 0);
			AttackTransition(filter, attack_Outside_1, PlayerActor::AttackType::Outside, 0);
			AttackTransition(filter, attack_Upper_1, PlayerActor::AttackType::Upper, 0);
			AttackTransition(filter, attack_Thrust_1, PlayerActor::AttackType::Thrust, 0);
							
			AttackTransition(filter, attack_Inside_2, PlayerActor::AttackType::Inside, 1);
			AttackTransition(filter, attack_Outside_2, PlayerActor::AttackType::Outside, 1);
			AttackTransition(filter, attack_Upper_2, PlayerActor::AttackType::Upper, 1);
			AttackTransition(filter, attack_Thrust_2, PlayerActor::AttackType::Thrust, 1);
							
			AttackTransition(filter, attack_Inside_3, PlayerActor::AttackType::Inside, 2);
			AttackTransition(filter, attack_Outside_3, PlayerActor::AttackType::Outside, 2);
			AttackTransition(filter, attack_Upper_3, PlayerActor::AttackType::Upper, 2);
			AttackTransition(filter, attack_Thrust_3, PlayerActor::AttackType::Thrust, 2);

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
			RockOn_Left_Filter->AddTransition(rockOn_Right_Run, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.1f, false);
				transition->AddConditionFloat(walkValue, Greater, 0.5f);
				transition->AddConditionInt(RockOnDirection, (int)PlayerActor::Direction::Right);
			});
			RockOn_Left_Filter->AddTransition(rockOn_Right_Walk, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.1f, false);
				transition->AddConditionFloat(walkValue, Greater, 0.1f);
				transition->AddConditionInt(RockOnDirection, (int)PlayerActor::Direction::Right);
			});
			RockOn_Left_Filter->AddTransition(rockOn_Back_Run, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.1f, false);
				transition->AddConditionFloat(walkValue, Greater, 0.5f);
				transition->AddConditionInt(RockOnDirection, (int)PlayerActor::Direction::Back);
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
			RockOn_Right_Filter->AddTransition(rockOn_Left_Run, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.1f, false);
				transition->AddConditionFloat(walkValue, Greater, 0.5f);
				transition->AddConditionInt(RockOnDirection, (int)PlayerActor::Direction::Left);
			});
			RockOn_Right_Filter->AddTransition(rockOn_Left_Walk, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.1f, false);
				transition->AddConditionFloat(walkValue, Greater, 0.1f);
				transition->AddConditionInt(RockOnDirection, (int)PlayerActor::Direction::Left);
			});
			RockOn_Right_Filter->AddTransition(rockOn_Back_Run, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.1f, false);
				transition->AddConditionFloat(walkValue, Greater, 0.5f);
				transition->AddConditionInt(RockOnDirection, (int)PlayerActor::Direction::Back);
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
			RockOn_Back_Filter->AddTransition(rockOn_Left_Run, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.1f, false);
				transition->AddConditionFloat(walkValue, Greater, 0.5f);
				transition->AddConditionInt(RockOnDirection, (int)PlayerActor::Direction::Left);
			});
			RockOn_Back_Filter->AddTransition(rockOn_Left_Walk, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.1f, false);
				transition->AddConditionFloat(walkValue, Greater, 0.1f);
				transition->AddConditionInt(RockOnDirection, (int)PlayerActor::Direction::Left);
			});
			RockOn_Back_Filter->AddTransition(rockOn_Right_Run, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.1f, false);
				transition->AddConditionFloat(walkValue, Greater, 0.5f);
				transition->AddConditionInt(RockOnDirection, (int)PlayerActor::Direction::Right);
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
			
			rockOn_Back_Step->AddTransition(land, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.05f, false);
				transition->AddConditionTrigger(step_Trigger_Land);
				//transition->AddConditionBool(isRockOn, true);
			});
			/*rockOn_Back_Step->AddTransition(idle, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.1f, false);
				transition->AddConditionTrigger(step_Trigger_Land);
				transition->AddConditionBool(isRockOn, false);
			});*/
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
		}
		// Weapon_Change
		{
			weapon_Change->AddTransition(idle, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.05f, true);
			});
		}

		// Attack_Filter
		{
			// 弾かれる
			Attack_Filter->AddTransition(attack_Flip, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.2f, false);
				transition->AddConditionTrigger(attack_Flip_Trigger);
			});
			Attack_Filter->AddTransition(rockOn_Idle, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.2f, true, 0.7f);
				transition->AddConditionFloat(walkValue, Less, 0.1f);
				transition->AddConditionBool(isRockOn, true);
			});
			Attack_Filter->AddTransition(idle, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.2f, true, 0.7f);
				transition->AddConditionFloat(walkValue, Less, 0.1f);
			});
			Attack_Filter->AddTransition(run, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.1f, true, 0.9f);
				transition->AddConditionFloat(walkValue, Greater, 0.5f);
			});
			Attack_Filter->AddTransition(walk, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.1f, true, 0.9f);
				transition->AddConditionFloat(walkValue, Greater, 0.1f);
			});
			Attack_Filter->AddTransition(fall, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.05f, false);
				transition->AddConditionBool(isFall, true);
			});
		}

		attack_Flip->AddTransition(idle, [=](std::shared_ptr<AnimationTransition> & transition)
		{
			transition->SetOption(0.1f, true, 0.8f);
			transition->AddConditionFloat(walkValue, Less, 0.1f);
		});
		attack_Flip->AddTransition(walk, [=](std::shared_ptr<AnimationTransition> & transition)
		{
			transition->SetOption(0.1f, true, 0.8f);
			transition->AddConditionFloat(walkValue, Greater, 0.1f);
		});

		// Idle_Filter
		{
			Idle_Filter->AddTransition(roll, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.05f, false);
				transition->AddConditionTrigger(rollTrigger);
			});
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
			Idle_Filter->AddTransition(RockOn_Filter, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.1f, false);
				transition->AddConditionTrigger(isRockOn);
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
					transition->SetOption(0.1f, false);
					transition->AddConditionFloat(walkValue, Greater, 0.6f);
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
					transition->AddConditionFloat(walkValue, Less, 0.3f);
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
				transition->SetOption(0.1f, true, 0.9f);
				transition->AddConditionFloat(walkValue, Less, 0.1f);
			});
			roll->AddTransition(run, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.1f, true, 0.9f);
				transition->AddConditionFloat(walkValue, Greater, 0.2f);
			});
			roll->AddTransition(walk, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.1f, true, 0.9f);
				transition->AddConditionFloat(walkValue, Greater, 0.1f);
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
				transition->SetOption(0.05f, true, 0.90f);
			});
			land->AddTransition(run, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.3f, false);
				transition->AddConditionFloat(walkValue, Greater, 0.5f);
			});
			land->AddTransition(walk, [=](std::shared_ptr<AnimationTransition> & transition)
			{
				transition->SetOption(0.2f, false);
				transition->AddConditionFloat(walkValue, Greater, 0.2f);
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
