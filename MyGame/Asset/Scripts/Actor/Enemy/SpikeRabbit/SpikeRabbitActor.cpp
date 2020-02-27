#include "SpikeRabbitActor.h"
#include "State/SpikeRabbitIdle.h"
#include "State/SpikeRabbitMove.h"
#include "State/SpikeRabbitAttackReady.h"
#include "State/SpikeRabbitAttack.h"
#include "State/SpikeRabbitAttackStop.h"
#include "State/SpikeRabbitAttackFinish.h"
#include "State/SpikeRabbitDamage.h"
#include "State/SpikeRabbitDeth.h"

#include "../../Player/PlayerActor.h"

void SpikeRabbitActor::DrawImGui(int id)
{
	std::string strId = "##SpikeRabbitActor" + std::to_string(id);
	ImGui::DragFloat(("Move Speed" + strId).c_str(), &moveSpeed);
	ImGui::DragFloat(("Attack Move Speed" + strId).c_str(), &attackMoveSpeed);
}

void SpikeRabbitActor::OnStart()
{
	animator = gameObject.lock()->GetComponent<Animator>();
	if (animator.expired())
	{
		animator = gameObject.lock()->GetComponentWithChild<Animator>();
	}
	rigidbody = gameObject.lock()->GetComponent<Rigidbody>();

	fsmManager = std::make_unique<FSMManager<SpikeRabbitActor>>();
	fsmManager->AddState((int)State::Idle, new SpikeRabbitIdle());
	fsmManager->AddState((int)State::Move, new SpikeRabbitMove());
	fsmManager->AddState((int)State::Attack_Ready, new SpikeRabbitAttackReady());
	fsmManager->AddState((int)State::Attack, new SpikeRabbitAttack());
	fsmManager->AddState((int)State::Attack_Stop, new SpikeRabbitAttackStop());
	fsmManager->AddState((int)State::Attack_Finish, new SpikeRabbitAttackFinish());
	fsmManager->AddState((int)State::Damage, new SpikeRabbitDamage());
	fsmManager->AddState((int)State::Deth, new SpikeRabbitDeth());

	ChangeState(State::Idle);
}

void SpikeRabbitActor::OnUpdate()
{
	state.lock()->OnUpdate(this);
}

void SpikeRabbitActor::OnLateUpdate()
{
	state.lock()->OnLateUpdate(this);
}

void SpikeRabbitActor::ChangeState(State state)
{
	if (!fsmManager) return;
	this->currentState = state;
	fsmManager->ChangeState(this, (int)state);
}

void SpikeRabbitActor::OnCollisionStay(std::weak_ptr<Collision>& mine, std::weak_ptr<Collision>& other)
{
	if (other.lock()->isTrigger) return;
	auto p = other.lock()->gameObject.lock()->GetComponent<PlayerActor>();
	if (!p.expired())
	{
		ChangeState(State::Attack_Stop);
	}
}

void SpikeRabbitActor::OnTriggerStay(std::weak_ptr<Collision>& mine, std::weak_ptr<Collision>& other)
{
	if (other.lock()->isTrigger) return;
	auto p = other.lock()->gameObject.lock()->GetComponent<PlayerActor>();
	if (!p.expired())
	{
		player = p;
	}
}

void SpikeRabbitActor::OnDestroy()
{
}
