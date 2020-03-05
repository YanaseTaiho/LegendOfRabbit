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
#include "../../../../DirectX/Common.h"

void SpikeRabbitActor::DrawImGui(int id)
{
	std::string strId = "##SpikeRabbitActor" + std::to_string(id);
	ImGui::DragFloat(("Move Speed" + strId).c_str(), &moveSpeed);
	ImGui::DragFloat(("Attack Move Speed" + strId).c_str(), &attackMoveSpeed);
	ImGui::DragFloat(("Ray Length" + strId).c_str(), &rayLength, 0.01f);

	ImGui::Text("Attack Collision");
	MyImGui::DropTargetComponent(attackCollision, strId);
}

void SpikeRabbitActor::Start()
{
	hitPoint = 3;
	playerFindTime = 0.0f;

	animator = gameObject.lock()->GetComponent<Animator>();
	if (animator.expired())
	{
		animator = gameObject.lock()->GetComponentWithChild<Animator>();
	}
	rigidbody = gameObject.lock()->GetComponent<Rigidbody>();

	// ダメージ用のカラーの初期化
	damageFlashColor.SetRenderer(this->gameObject.lock()->GetComponentWithChild<Renderer>());

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

void SpikeRabbitActor::Update()
{
	// 視角から離れてから一定の時間で見失う
	if (!player.expired() && currentState != State::Attack_Ready)
	{
		if (playerFindTime <= 0) player.reset();
		playerFindTime -= Time::DeltaTime();
	}

	state.lock()->OnUpdate(this);

	damageFlashColor.OnUpdate();
}

void SpikeRabbitActor::LateUpdate()
{
	state.lock()->OnLateUpdate(this);

	CheckGround();
}

void SpikeRabbitActor::ChangeState(State state)
{
	if (!fsmManager) return;
	this->currentState = state;
	fsmManager->ChangeState(this, (int)state);
}

void SpikeRabbitActor::OnCollisionStay(std::weak_ptr<Collision>& mine, std::weak_ptr<Collision>& other)
{
	if (currentState != State::Attack && currentState != State::Attack_Ready) return;

	if (other.lock()->isTrigger) return;
	if (mine.lock() != attackCollision.lock()) return;

	auto p = other.lock()->gameObject.lock()->GetComponent<PlayerActor>();
	if (!p.expired())
	{
		DamageType type = p.lock()->Damage(1, other, mine);
		if (type == DamageType::None) return;

		ChangeState(State::Attack_Stop);
	}
	else if(currentState == State::Attack)
	{
		auto effect = Singleton<GameObjectManager>::Instance()->Instantiate(Singleton<SceneManager>::Instance()->GetCurrentScene()->GetPrefabGameObject("Effect"));
		if (!effect.expired())
		{
			effect.lock()->transform.lock()->SetWorldPosition(mine.lock()->transform.lock()->GetWorldPosition());
			effect.lock()->GetComponent<EffekseerSystem>().lock()->type = EffekseerType::Hit01;
			effect.lock()->GetComponent<EffekseerSystem>().lock()->Play();
		}
		rigidbody.lock()->AddForce(-transform.lock()->forward() * 5.0f);
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
		playerFindTime = 3.0f;
	}
}

DamageType SpikeRabbitActor::Damage(int damage, std::weak_ptr<Collision>& mine, std::weak_ptr<Collision>& other, Vector3 force)
{
	if (currentState == State::Damage || currentState == State::Deth) return DamageType::None;

	rigidbody.lock()->AddForce(force);

	hitPoint -= damage;
	if (hitPoint <= 0)
	{
		damageFlashColor.StartFlash(Color::white(), Color::red(), 1.0f, 5.0f);
		ChangeState(State::Deth);
		return DamageType::Deth;
	}
	else
	{
		damageFlashColor.StartFlash(Color::white(), Color::red(), 1.0f, 0.5f);
		ChangeState(State::Damage);
	}

	return DamageType::Hit;
}

void SpikeRabbitActor::OnDestroy()
{
}

void SpikeRabbitActor::CheckGround()
{
	Ray downRay;
	RayCastInfo castInfo;
	Vector3 pos = transform.lock()->GetWorldPosition();
	downRay.Set(pos, Vector3::down(), rayLength);
	DebugLine::DrawRay(downRay.start, downRay.end, Color::yellow());

	if (rigidbody.expired()) return;

	if (RayCast::JudgeAllCollision(&downRay, &castInfo, rigidbody.lock()->collisions))
	{
		auto other = castInfo.collision.lock()->gameObject.lock()->GetComponent<BaseActor>();
		if (other.expired())	// アクターは対象外
		{
			float h = castInfo.point.y + rayLength;
			{
				pos.y = Mathf::Lerp(pos.y, h - 0.1f, 0.3f); // レイの先端より少し下にする

				transform.lock()->SetWorldPosition(pos);
				if (rigidbody.lock()->velocity.y < 0.0f)
				{
					rigidbody.lock()->velocity.y = 0.0f;
				}
			}
		}
	}
}
