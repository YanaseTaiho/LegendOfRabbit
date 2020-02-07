#include "PlayerActor.h"
#include "State/PlayerIdle.h"
#include "State/PlayerMove.h"
#include "State/PlayerAir.h"
#include "State/PlayerRoll.h"
#include "State/PlayerRollStop.h"
#include "State/PlayerCliffGrap.h"
#include "State/PlayerCliffJump.h"
#include "State/PlayerAttackJump.h"
#include "State/PlayerAttack.h"
#include "State/PlayerStep.h"

#include "../../Camera/CameraController.h"
#include "../../RotationFixedController.h"
#include "../../LocusController.h"
#include "../../TargetImageController.h"
#include "../../../DirectX/Common.h"

using namespace MyDirectX;

void PlayerActor::DrawImGui(int id)
{
	std::string strId = "##PlayerActor" + std::to_string(id);
	ImGui::Text("Camera Controller");
	MyImGui::DropTargetComponent(cameraController, strId);
	ImGui::Text("Sorwd Handler");
	MyImGui::DropTargetComponent(sword_HandContorller, strId);
	ImGui::Text("Shield Handler");
	MyImGui::DropTargetComponent(shield_HandContorller, strId);
	ImGui::Text("Shield Rock Handler");
	MyImGui::DropTargetComponent(shieldRock_HandContorller, strId);

	ImGui::Text("Hand Sword");
	MyImGui::DropTargetGameObject(handSword, strId);
	ImGui::Text("Hand Shield");
	MyImGui::DropTargetGameObject(handShield, strId);
	ImGui::Text("Back Sword");
	MyImGui::DropTargetGameObject(backSword, strId);
	ImGui::Text("Back Shield");
	MyImGui::DropTargetGameObject(backShield, strId);

	ImGui::Text("Locus Controller");
	MyImGui::DropTargetComponent(locusController, strId);

	ImGui::Text("Target Image Transform");
	MyImGui::DropTargetComponent(targetImageController, strId);

	ImGui::DragFloat(("Ray Start" + strId).c_str(), &rayStart, 0.01f);
	ImGui::DragFloat(("Ray Length" + strId).c_str(), &rayLength, 0.01f);
	ImGui::DragFloat(("Height" + strId).c_str(), &height, 0.01f);
	ImGui::DragFloat(("Jump Force" + strId).c_str(), &jumpForce, 0.01f);
	ImGui::DragFloat(("Move Force" + strId).c_str(), &moveForce, 0.01f);
	ImGui::DragFloat(("Move Max" + strId).c_str(), &forceMax, 0.01f);

	ImGui::Text("Cliff");
	ImGui::DragFloat(("Cliff Jump Force" + strId).c_str(), &cliffJumpForce, 0.01f);
	ImGui::DragFloat(("Cliff RayStartY Front" + strId).c_str(), &cliffRayStartY_Front, 0.01f);
	ImGui::DragFloat(("Cliff RayLength Front" + strId).c_str(), &cliffRayLength_Front, 0.01f);
	ImGui::DragFloat(("Cliff RayStartY Down" + strId).c_str(), &cliffRayStartY_Down, 0.01f);
	ImGui::DragFloat(("Cliff RayStartZ Down" + strId).c_str(), &cliffRayStartZ_Down, 0.01f);
}

PlayerActor::PlayerActor() : horizontalRegistance(0.8f)
{
}

PlayerActor::~PlayerActor()
{
}

void PlayerActor::OnStart()
{
	if (fsmManager) return;

	fsmManager = std::make_unique<FSMManager<PlayerActor>>();
	fsmManager->AddState((int)State::Idle, new PlayerIdle());
	fsmManager->AddState((int)State::Move, new PlayerMove());
	fsmManager->AddState((int)State::Air, new PlayerAir());
	fsmManager->AddState((int)State::Roll, new PlayerRoll());
	fsmManager->AddState((int)State::RollStop, new PlayerRollStop());
	fsmManager->AddState((int)State::CliffGrap, new PlayerCliffGrap());
	fsmManager->AddState((int)State::CliffJump, new PlayerCliffJump());
	fsmManager->AddState((int)State::AttackJump, new PlayerAttackJump());
	fsmManager->AddState((int)State::Attack, new PlayerAttack());
	fsmManager->AddState((int)State::Step, new PlayerStep());

	animator = this->gameObject.lock()->GetComponent<Animator>();
	rigidbody = this->gameObject.lock()->GetComponent<Rigidbody>();

	ChangeState(State::Idle);

	// 剣の当たり判定の設定
	if (!this->locusController.expired())
	{
		this->locusController.lock()->SetCollision(this->gameObject, [=](MeshCastInfo hitInfo) { AttackSwordHit(hitInfo); });
	}
}

void PlayerActor::OnUpdate()
{
	if (Input::Keyboad::IsTrigger('F')
		|| GamePad::IsTrigger(GamePad::Button::LEFT_SHOULDER))
	{
		isRockOn = !isRockOn;

		animator.lock()->SetBool("IsRockOn", isRockOn);
		if (isRockOn)
		{
			targetTransform.reset();
			cameraController.lock()->ChangePlugin(CameraController::Plugin::RockOn);
		}
		else
		{
			targetTransform.reset();
			cameraController.lock()->ChangePlugin(CameraController::Plugin::Character);
		}
	}

	if (animator.lock()->IsCurrentAnimation("Idle")
		|| animator.lock()->IsCurrentAnimation("Walk")
		|| animator.lock()->IsCurrentAnimation("Run")
		|| animator.lock()->IsCurrentAnimation("Land")
		|| animator.lock()->IsCurrentAnimation("RockOn_Left_Step")
		|| animator.lock()->IsCurrentAnimation("RockOn_Right_Step")
		//|| animator.lock()->IsCurrentAnimation("RockOn_Back_Step")
		|| animator.lock()->IsCurrentAnimation("RockOn_Filter")
		|| animator.lock()->IsCurrentAnimation("RockOn_Left_Filter")
		|| animator.lock()->IsCurrentAnimation("RockOn_Right_Filter")
		|| animator.lock()->IsCurrentAnimation("RockOn_Back_Filter"))
	{
		if (!handSword.expired() && handSword.lock()->IsActive())
		{
			if (!sword_HandContorller.expired()) sword_HandContorller.lock()->SetWeight(0.1f);
		}
		
		if (!handShield.expired() && handShield.lock()->IsActive())
		{
			if (!shield_HandContorller.expired()) shield_HandContorller.lock()->SetWeight(0.1f);

			if (!shieldRock_HandContorller.expired())
			{
				if (Input::Keyboad::IsPress('2')
					|| GamePad::IsPress(GamePad::Button::RIGHT_SHOULDER))
				{
					shieldRock_HandContorller.lock()->SetWeight(0.3f);
				}
				else
				{
					shieldRock_HandContorller.lock()->SetWeight(-0.3f);
				}
			}
		}
	}
	else
	{
		if (!handSword.expired() && handSword.lock()->IsActive())
		{
			if (!sword_HandContorller.expired()) sword_HandContorller.lock()->SetWeight(-0.1f);
		}

		if (!handShield.expired() && handShield.lock()->IsActive())
		{
			if (!shield_HandContorller.expired()) shield_HandContorller.lock()->SetWeight(-0.1f);

			if (!shieldRock_HandContorller.expired()) shieldRock_HandContorller.lock()->SetWeight(-0.3f);
		}
	}

	UpdateInput();
	// 地面に接しているか
	CheckGround();
	// 前に崖があるのかチェック
	CheckCliff();

	if (!state.expired())
		state.lock()->OnUpdate(this);

	// 移動速度の最大値
	Vector2 force = Vector2(rigidbody.lock()->velocity.x, rigidbody.lock()->velocity.z);
	float forceLen = force.Length();
	if (forceLen > forceMax)
	{
		force = force.Normalized() * forceMax;
		forceLen = forceMax;
	}

	force *= horizontalRegistance;
	rigidbody.lock()->velocity = Vector3(force.x, rigidbody.lock()->velocity.y, force.y);

	forceAmount = forceLen / forceMax;

	// ターゲット画像の設定
	if (!targetTransform.expired() && !targetImageController.expired())
	{
		if (!targetImageController.lock()->gameObject.lock()->IsActive())
		{
			targetImageController.lock()->TargetStart();
			targetImageController.lock()->gameObject.lock()->SetActive(true);
		}
		targetImageController.lock()->transform.lock()->SetWorldPosition(targetTransform.lock()->GetWorldPosition());
	}
	else if(!targetImageController.expired())
	{
		targetImageController.lock()->gameObject.lock()->SetActive(false);
	}
	targetTransform.reset();
}

void PlayerActor::OnLateUpdate()
{
	if (!state.expired())
		state.lock()->OnLateUpdate(this);
}

void PlayerActor::Draw()
{
	//CheckGround();
	//CheckCliff();
}

void PlayerActor::OnCollisionStay(std::weak_ptr<Collision>& mine, std::weak_ptr<Collision>& other)
{
}

void PlayerActor::OnTriggerStay(std::weak_ptr<Collision>& mine, std::weak_ptr<Collision>& other)
{
	if (targetTransform.expired())
	{
		auto actor = other.lock()->gameObject.lock()->GetComponent<BaseActor>();
		if (!actor.expired())
		{
			targetTransform = actor.lock()->transform;

			//if (!targetImageController.expired())
			//{
			//	targetImageController.lock()->TargetStart();
			//	//targetImageController.lock()->gameObject.lock()->SetActive(true);
			//}
		}
	}
}

void PlayerActor::ChangeState(State state)
{
	if (!fsmManager) return;
	currentState = state;
	fsmManager->ChangeState(this, (int)state);
}

void PlayerActor::WeaponHold(std::function<void()> func)
{
	if (isWeaponHold || animator.lock()->IsCurrentAnimation("Weapon_Change")) return;

	animator.lock()->SetTrigger("Weapon_Change_Trigger");

	animator.lock()->SetAnimationCallBack("Weapon_Change", 14, [=]()
	{
		isWeaponHold = true;

		if (!sword_HandContorller.expired()) sword_HandContorller.lock()->SetWeight(0.1f);
		if (!handSword.expired()) handSword.lock()->SetActive(isWeaponHold);
		if (!backSword.expired()) backSword.lock()->SetActive(!isWeaponHold);

		if (!shield_HandContorller.expired()) shield_HandContorller.lock()->SetWeight(0.1f);
		if (!handShield.expired()) handShield.lock()->SetActive(isWeaponHold);
		if (!backShield.expired()) backShield.lock()->SetActive(!isWeaponHold);

		if(func) func();
	});
}

void PlayerActor::WeaponNotHold()
{
	if (!isWeaponHold || animator.lock()->IsCurrentAnimation("Weapon_Change")) return;

	animator.lock()->SetTrigger("Weapon_Change_Trigger");

	animator.lock()->SetAnimationCallBack("Weapon_Change", 14, [=]()
	{
		isWeaponHold = false;

		if (!sword_HandContorller.expired()) sword_HandContorller.lock()->SetWeight(-0.1f);
		if (!handSword.expired()) handSword.lock()->SetActive(isWeaponHold);
		if (!backSword.expired()) backSword.lock()->SetActive(!isWeaponHold);

		if (!shield_HandContorller.expired()) shield_HandContorller.lock()->SetWeight(-0.1f);
		if (!handShield.expired()) handShield.lock()->SetActive(isWeaponHold);
		if (!backShield.expired()) backShield.lock()->SetActive(!isWeaponHold);
	});
}

void PlayerActor::AttackSwordHit(MeshCastInfo hitInfo)
{
	if (hitInfo.collision.lock()->isTrigger) return;

	auto other = hitInfo.collision.lock()->gameObject.lock()->GetComponent<BaseActor>();
	if (!other.expired())
	{
		auto rb = other.lock()->gameObject.lock()->GetComponent<Rigidbody>();
		if (!rb.expired())
		{
			Vector3 dir = other.lock()->transform.lock()->GetWorldPosition() - transform.lock()->GetWorldPosition();
			rb.lock()->AddForce(dir.Normalized() * 7.0f);
		}
		GameObject::Destroy(other.lock()->gameObject, 0.5f);
	}
	// 地面は判定しない
	else if (Vector3::Dot(Vector3::up(), hitInfo.normal) < 0.4f)
	{
		Vector3 normal = hitInfo.normal;
		normal.y = 0.0f;
		normal.Normalize();
		rigidbody.lock()->AddForce(normal * 200.0f);
	}
}

void PlayerActor::UpdateInput()
{
	if (cameraController.expired()) return;

	float h = 0.0f;
	float v = 0.0f;
	bool input = false;
	float inputForce = 0.04f;

	if (Input::Keyboad::IsPress('W')) {
		v += inputForce; input = true;
	}
	if (Input::Keyboad::IsPress('S')) {
		v -= inputForce; input = true;
	}
	if (Input::Keyboad::IsPress('A')) {
		h -= inputForce; input = true;
	}
	if (Input::Keyboad::IsPress('D')) {
		h += inputForce; input = true;
	}

	if (input)
	{
		moveAmount += inputForce;
		vertical = v;
		horizontal = h;
		if (moveAmount > 1.0f) moveAmount = 1.0f;
	}
	else
	{
		moveAmount *= 0.5f;
	}

	// ゲームパッドの入力
	float padX = GamePad::ThumbLX();
	float padY = GamePad::ThumbLY();
	float absPadX = abs(padX);
	float absPadY = abs(padY);

	if (absPadX > 0.15f || absPadY > 0.15f)
	{
		moveAmount = Mathf::Clamp01(absPadX + absPadY);
		horizontal = padX;
		vertical = padY;
	}

	if (moveAmount > 0.1f)
	{
		Vector3 forward = cameraController.lock()->transform.lock()->forward();
		forward.y = 0.0f;
		forward.Normalize();
		Vector3 right = cameraController.lock()->transform.lock()->right();
		right.y = 0.0f;
		right.Normalize();

		moveDir = forward * vertical + right * horizontal;
		moveDir.Normalize();
	}
	else
	{
		moveDir = Vector3::zero();
	}

//	float v = Mathf::Clamp(vertical, -1.0f, 1.0f);
//	float h = Mathf::Clamp(horizontal, -1.0f, 1.0f);

//	moveAmount = Mathf::Clamp01(Mathf::Absf(v) + Mathf::Absf(h));
}

void PlayerActor::CheckGround()
{
	onGround = false;
	horizontalRegistance = 1.0f;
	
	Ray downRay;
	Vector3 pos = transform.lock()->GetWorldPosition();
	downRay.Set(pos + Vector3(0.0f, rayStart, 0.0f), Vector3::down(), rayLength);
	DebugLine::DrawRay(downRay.start, downRay.end, Color::yellow());

	if (rigidbody.expired()) return;

	if (RayCast::JudgeAllCollision(&downRay, &castGroundInfo, this->gameObject))
	{
		float h = castGroundInfo.point.y + height;
		//if (h + 1.0f >= pos.y)
		{
			pos.y = Mathf::Lerp(pos.y, h - 0.1f, 0.3f);

			transform.lock()->SetWorldPosition(pos);
			if (rigidbody.lock()->velocity.y < 0.0f)
			{
				rigidbody.lock()->velocity.y = 0.0f;
			}
		}
		onGround = true;
		horizontalRegistance = 0.7f;
	}

	animator.lock()->SetBool("IsFall", !onGround);
}

void PlayerActor::CheckCliff()
{
	castCliffWallInfo.collision.reset();
	castCliffGroundInfo.collision.reset();

	Ray ray;
	Vector3 pos = transform.lock()->GetWorldPosition();
	
	// 前方に飛ばすレイ
	ray.Set(pos + Vector3(0.0f, cliffRayStartY_Front, 0.0f), transform.lock()->forward(), cliffRayLength_Front);
	DebugLine::DrawRay(ray.start, ray.end, Color::red());
	if (RayCast::JudgeAllCollision(&ray, &castCliffWallInfo, this->gameObject))
	{
	}
	// 前方の下に飛ばすレイ
	ray.Set(pos + Vector3(0.0f, cliffRayStartY_Down, 0.0f) + transform.lock()->forward() * cliffRayStartZ_Down, Vector3::down(), cliffRayStartY_Down - cliffRayStartY_Front);
	DebugLine::DrawRay(ray.start, ray.end, Color::red());
	if (RayCast::JudgeAllCollision(&ray, &castCliffGroundInfo, this->gameObject))
	{

	}
}
