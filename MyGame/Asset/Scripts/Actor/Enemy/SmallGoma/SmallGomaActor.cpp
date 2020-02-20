#include "SmallGomaActor.h"
#include "State/SmallGomaIdle.h"
#include "State/SmallGomaMove.h"
#include "State/SmallGomaChase.h"
#include "../../Player/PlayerActor.h"

void SmallGomaActor::DrawImGui(int id)
{
	std::string strId = "##SmallGomaActor" + std::to_string(id);
	ImGui::DragFloat(("Move Speed" + strId).c_str(), &moveSpeed);
}

void SmallGomaActor::OnStart()
{
	animator = gameObject.lock()->GetComponent<Animator>();
	rigidbody = gameObject.lock()->GetComponent<Rigidbody>();

	fsmManager = std::make_unique<FSMManager<SmallGomaActor>>();
	fsmManager->AddState((int)State::Idle, new SmallGomaIdle());
	fsmManager->AddState((int)State::Move, new SmallGomaMove());
	fsmManager->AddState((int)State::Chase, new SmallGomaChase());

	ChangeState(State::Idle);
}

void SmallGomaActor::OnUpdate()
{
	state.lock()->OnUpdate(this);

	player.reset();
}

void SmallGomaActor::OnLateUpdate()
{
	state.lock()->OnLateUpdate(this);
}

void SmallGomaActor::ChangeState(State state)
{
	if (!fsmManager) return;
	this->currentState = state;
	fsmManager->ChangeState(this, (int)state);
}

void SmallGomaActor::OnCollisionStay(std::weak_ptr<Collision>& mine, std::weak_ptr<Collision>& other)
{
}

void SmallGomaActor::OnTriggerStay(std::weak_ptr<Collision>& mine, std::weak_ptr<Collision>& other)
{
	if (other.lock()->isTrigger) return;
	auto p = other.lock()->gameObject.lock()->GetComponent<PlayerActor>();
	if (!p.expired())
	{
		player = p;
	}
}

void SmallGomaActor::OnDestroy()
{
	auto effect = Singleton<GameObjectManager>::Instance()->Instantiate(Singleton<SceneManager>::Instance()->GetCurrentScene()->GetPrefabGameObject("Effect_Deth"));
	if (!effect.expired())
	{
		Singleton<AudioClipManager>::Instance()->Play(AudioData::SE_Bomb);
		effect.lock()->transform.lock()->SetWorldPosition(transform.lock()->GetWorldPosition());
	}
}
