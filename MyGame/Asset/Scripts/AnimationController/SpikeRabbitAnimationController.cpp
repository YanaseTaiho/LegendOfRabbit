#include "SpikeRabbitAnimationController.h"

void SpikeRabbitAnimationController::Initialize()
{
	AnimationController::Initialize();

	//auto IdleTrigger = AddParameterTrigger("IdleTrigger", false);
	//auto MoveTrigger = AddParameterTrigger("MoveTrigger", false);
	//auto AttackTrigger = AddParameterTrigger("AttackTrigger", false);
	//auto DamageTrigger = AddParameterTrigger("DamageTrigger", false);
	//auto DethTrigger = AddParameterTrigger("DethTrigger", false);

	AddFilter("Normal", [=](std::shared_ptr<AnimationFilter> & filter)
	{
		//======================= ステート追加 =======================//

		auto Idle = filter->AddState("Idle");
		auto Move = filter->AddState("Move");
		auto Attack = filter->AddState("Attack");
		auto Attack_Ready = filter->AddState("Attack_Ready");
		auto Attack_Stop = filter->AddState("Attack_Stop");
		auto Attack_Finish = filter->AddState("Attack_Finish");
		auto Damage = filter->AddState("Damage");
		auto Deth = filter->AddState("Deth");

		// エントリーセット
		filter->SetEntryPoint(Idle);

		//======================= トランジション追加 =======================//

		// ダメージは全ての状態から遷移可能
		filter->AddTransition(Idle, [=](std::shared_ptr<AnimationTransition> & transition)
		{
			transition->SetOption(0.2f, false, 1.0f, false);
		});
		filter->AddTransition(Move, [=](std::shared_ptr<AnimationTransition> & transition)
		{
			transition->SetOption(0.2f, false, 1.0f, false);
		});

		filter->AddTransition(Attack_Ready, [=](std::shared_ptr<AnimationTransition> & transition)
		{
			transition->SetOption(0.1f, false, 1.0f, false);
		});
		Attack_Ready->AddTransition(Attack, [=](std::shared_ptr<AnimationTransition> & transition)
		{
			transition->SetOption(0.1f, false, 1.0f, false);
		});
		Attack->AddTransition(Attack_Stop, [=](std::shared_ptr<AnimationTransition> & transition)
		{
			transition->SetOption(0.2f, false, 1.0f, false);
		});
		Attack->AddTransition(Attack_Finish, [=](std::shared_ptr<AnimationTransition> & transition)
		{
			transition->SetOption(0.2f, false, 1.0f, false);
		});

		filter->AddTransition(Damage, [=](std::shared_ptr<AnimationTransition> & transition)
		{
			transition->SetOption(0.1f, false, 1.0f, false);
		});
		filter->AddTransition(Deth, [=](std::shared_ptr<AnimationTransition> & transition)
		{
			transition->SetOption(0.1f, false, 1.0f, false);
		});
	});
}
