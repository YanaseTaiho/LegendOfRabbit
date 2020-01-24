#include "PlayerActor.h"
#include "State/PlayerIdle.h"
#include "State/PlayerMove.h"
#include "State/PlayerAir.h"
#include "State/PlayerCliffGrap.h"
#include "../../RotationFixedController.h"
#include "../../../DirectX/Common.h"

using namespace MyDirectX;

void PlayerActor::DrawImGui(int id)
{
	std::string strId = "##PlayerActor" + std::to_string(id);
	ImGui::Text("Camera Transform");
	MyImGui::DropTargetComponent(cameraTransform, strId);
	ImGui::Text("Sorwd Hand");
	MyImGui::DropTargetComponent(sorwd_HandContorller, strId);
	ImGui::Text("Shield Hand");
	MyImGui::DropTargetComponent(shield_HandContorller, strId);
	ImGui::Text("Shield Rock Hand");
	MyImGui::DropTargetComponent(shieldRock_HandContorller, strId);

	ImGui::Text("Hand Sword");
	MyImGui::DropTargetGameObject(handSword, strId);
	ImGui::Text("Hand Shield");
	MyImGui::DropTargetGameObject(handShield, strId);
	ImGui::Text("Back Sword");
	MyImGui::DropTargetGameObject(backSword, strId);
	ImGui::Text("Back Shield");
	MyImGui::DropTargetGameObject(backShield, strId);

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

PlayerActor::PlayerActor()
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
	fsmManager->AddState((int)State::CliffGrap, new PlayerCliffGrap());

	animator = this->gameObject.lock()->GetComponent<Animator>();
	rigidbody = this->gameObject.lock()->GetComponent<Rigidbody>();

	ChangeState(State::Idle);

	/*std::weak_ptr<GameObject> obj = System::FindGameObject(Tag::MainCamera);
	if (!obj.expired())
	{
		cameraTransform = obj.lock()->transform;
	}*/
}

void PlayerActor::OnUpdate()
{
	UpdateInput();

	if (!state.expired())
		state.lock()->OnUpdate(this);

	// 地面に接しているか
	CheckGround();
	// 前に崖があるのかチェック
	CheckCliff();

	// 移動速度の最大値
	Vector2 force = Vector2(rigidbody.lock()->velocity.x, rigidbody.lock()->velocity.z);
	float forceLen = force.Length();
	if (forceLen > forceMax)
	{
		force = force.Normalized() * forceMax;
		forceLen = forceMax;
		rigidbody.lock()->velocity = Vector3(force.x, rigidbody.lock()->velocity.y, force.y);
	}

	forceAmount = forceLen / forceMax;

	static bool isSorwd = false;
	static bool isShield = false;
	static bool isShieldRock = false;
	if (Input::Keyboad::IsTrigger('1'))
	{
		isSorwd = !isSorwd; 
		isShield = !isShield;

		if (!sorwd_HandContorller.expired()) sorwd_HandContorller.lock()->SetWeight(isSorwd ? 0.1f : -0.1f);
		if (!handSword.expired()) handSword.lock()->SetActive(isSorwd);
		if (!backSword.expired()) backSword.lock()->SetActive(!isSorwd);

		if (!shield_HandContorller.expired()) shield_HandContorller.lock()->SetWeight(isShield ? 0.1f : -0.1f);
		if (!handShield.expired()) handShield.lock()->SetActive(isShield);
		if (!backShield.expired()) backShield.lock()->SetActive(!isShield);
	}
	
	if (!handSword.expired() && handSword.lock()->IsActive())
	{
		static int combo = 0;

		if (animator.lock()->IsCurrentAnimation("Idle")
			|| animator.lock()->IsCurrentAnimation("Walk")
			|| animator.lock()->IsCurrentAnimation("Run"))
		{
			combo = 0;

			if (!sorwd_HandContorller.expired()) sorwd_HandContorller.lock()->SetWeight(0.1f);
		}
		else
		{
			if (!sorwd_HandContorller.expired()) sorwd_HandContorller.lock()->SetWeight(-0.1f);
		}

		
		if (Input::Keyboad::IsTrigger('E'))
		{
			animator.lock()->SetTrigger("Attack_Trigger");
			animator.lock()->SetInt("Attack_Type", (int)AttackType::Inside);
			animator.lock()->SetInt("Attack_Combo", combo);
			combo++;
			combo = Mathf::Loop(combo, 0, 2);
		}
	}
	if (!handShield.expired() && handShield.lock()->IsActive())
	{
		if (animator.lock()->IsCurrentAnimation("Idle")
			|| animator.lock()->IsCurrentAnimation("Walk")
			|| animator.lock()->IsCurrentAnimation("Run"))
		{
			if (!shield_HandContorller.expired()) shield_HandContorller.lock()->SetWeight(0.1f);
		}
		else
		{
			if (!shield_HandContorller.expired()) shield_HandContorller.lock()->SetWeight(-0.1f);
		}

		if (Input::Keyboad::IsPress('2'))
		{
			if (!shieldRock_HandContorller.expired()) shieldRock_HandContorller.lock()->SetWeight(0.2f);
		}
		else
		{
			if (!shieldRock_HandContorller.expired()) shieldRock_HandContorller.lock()->SetWeight(-0.2f);
		}
	}
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

void PlayerActor::ChangeState(State state)
{
	if (!fsmManager) return;
	currentState = state;
	fsmManager->ChangeState(this, (int)state);
}

void PlayerActor::UpdateInput()
{
	if (cameraTransform.expired()) return;

	float h = 0.0f;
	float v = 0.0f;
	bool input = false;
	float inputForce = 0.02f;

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

	//if (Mathf::Absf(vertical) < 0.01f) vertical = 0.0f;
	//if (Mathf::Absf(horizontal) < 0.01f) horizontal = 0.0f;

	Vector3 forward = cameraTransform.lock()->forward();
	forward.y = 0.0f;
	forward.Normalize();
	Vector3 right = cameraTransform.lock()->right();
	right.y = 0.0f;
	right.Normalize();

	moveDir = forward * vertical + right * horizontal;
	moveDir.Normalize();

//	float v = Mathf::Clamp(vertical, -1.0f, 1.0f);
//	float h = Mathf::Clamp(horizontal, -1.0f, 1.0f);

//	moveAmount = Mathf::Clamp01(Mathf::Absf(v) + Mathf::Absf(h));
}

void PlayerActor::CheckGround()
{
	onGround = false;
	
	Ray downRay;
	Vector3 pos = transform.lock()->GetWorldPosition();
	downRay.Set(pos + Vector3(0.0f, rayStart, 0.0f), Vector3::down(), rayLength);
	DebugLine::DrawRay(downRay.start, downRay.end, Color::yellow());

	if (rigidbody.expired()) return;

	if (RayCast::JudgeAllCollision(&downRay, &castGroundInfo))
	{
		float h = castGroundInfo.point.y + height;
		if (h + 1.0f >= pos.y)
		{
			pos.y = Mathf::Lerp(pos.y, h - 0.1f, 0.3f);

			transform.lock()->SetWorldPosition(pos);
			if (rigidbody.lock()->velocity.y < 0.0f)
			{
				rigidbody.lock()->velocity.y = 0.0f;
			}
		}
		onGround = true;
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
	if (RayCast::JudgeAllCollision(&ray, &castCliffWallInfo))
	{
	}
	// 前方の下に飛ばすレイ
	ray.Set(pos + Vector3(0.0f, cliffRayStartY_Down, 0.0f) + transform.lock()->forward() * cliffRayStartZ_Down, Vector3::down(), cliffRayStartY_Down - cliffRayStartY_Front);
	DebugLine::DrawRay(ray.start, ray.end, Color::red());
	if (RayCast::JudgeAllCollision(&ray, &castCliffGroundInfo))
	{

	}
}
