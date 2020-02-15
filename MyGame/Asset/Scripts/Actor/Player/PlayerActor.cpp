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
#include "../../CollisionObserver.h"
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

	// ロックオン対象についての処理
	auto observer = this->gameObject.lock()->GetComponent<CollisionObserver>();
	if (!observer.expired())
	{
		//	相手がプレイヤーのロックオン範囲内に入った時
		observer.lock()->SetTriggerEnter([=](std::weak_ptr<Collision> other)
		{
			auto actor = other.lock()->gameObject.lock()->GetComponent<BaseActor>();
			if (actor.expired()) return;
			targetTriggerList.emplace_back(actor);
		});

		//	相手がプレイヤーのロックオンから外れた時
		observer.lock()->SetTriggerExit([=](std::weak_ptr<Collision> other)
		{
			auto actor = other.lock()->gameObject.lock()->GetComponent<BaseActor>();
			if (actor.expired()) return;

			// ターゲット中の相手ならロックオン解除
			if (isRockOn && !targetTransform.expired()
				&& targetTransform.lock()->gameObject.lock() == other.lock()->gameObject.lock())
			{
				RockOn(false);
			}
			
			for (auto itr = targetTriggerList.begin(), end = targetTriggerList.end(); itr != end; ++itr)
			{
				if (itr->lock() == actor.lock())
				{
					targetTriggerList.erase(itr);
					return;
				}
			}
		});
	}

	// アニメーションに応じてのSE再生用
	animator.lock()->SetAnimationCallBack("Walk", 8, []()
	{
		Singleton<AudioClipManager>::Instance()->Play(AudioData::SE_SwordChakin);
	});
	animator.lock()->SetAnimationCallBack("Walk", 23, []()
	{
		Singleton<AudioClipManager>::Instance()->Play(AudioData::SE_SwordChakin);
	});
	animator.lock()->SetAnimationCallBack("Run", 8, []()
	{
		Singleton<AudioClipManager>::Instance()->Play(AudioData::SE_SwordChakin);
	});
	animator.lock()->SetAnimationCallBack("Run", 23, []()
	{
		Singleton<AudioClipManager>::Instance()->Play(AudioData::SE_SwordChakin);
	});
}

void PlayerActor::OnUpdate()
{
	// リストにある対象が既に死んでいないかチェック
	for (auto itr = targetTriggerList.begin(), end = targetTriggerList.end(); itr != end;)
	{
		if (itr->expired()) itr = targetTriggerList.erase(itr);
		else ++itr;
	}

	ImGui::Text("TargetTrigger");
	ImGui::Indent();
	for (auto & c : targetTriggerList)
	{
		ImGui::Text(c.lock()->gameObject.lock()->name.c_str());
	}
	ImGui::Unindent();

	// ロックオン処理
	if (Input::Keyboad::IsTrigger('F')
		|| GamePad::IsTrigger(GamePad::Button::LEFT_SHOULDER))
	{
		RockOn(!isRockOn);
	}
	if (isRockOn)
	{
		//  ターゲットがいない場合は長押ししていないとロックオン状態を解除する
		if (targetTransform.expired())
		{
			if (!GamePad::IsPress(GamePad::Button::LEFT_SHOULDER))
				RockOn(false);
		}
		// ターゲット切り替え
		else if (GamePad::IsTrigger(GamePad::Button::THUMB_R_LEFT))
		{
			SetNextTarget(false);
		}
		else if (GamePad::IsTrigger(GamePad::Button::THUMB_R_RIGHT))
		{
			SetNextTarget(true);
		}
	}

	// ターゲット画像の設定
	if (!targetImageController.expired())
	{
		if (!targetTransform.expired())
		{
			if (!targetImageController.lock()->gameObject.lock()->IsActive())
			{
				targetImageController.lock()->TargetStart();
				targetImageController.lock()->gameObject.lock()->SetActive(true);
			}
			targetImageController.lock()->transform.lock()->SetWorldPosition(targetTransform.lock()->GetWorldPosition());
		}
		else
		{
			targetImageController.lock()->gameObject.lock()->SetActive(false);
		}
	}

	// 装備している時のボーンの位置操作
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
					shieldRock_HandContorller.lock()->SetWeight(0.2f);
				}
				else
				{
					shieldRock_HandContorller.lock()->SetWeight(-0.2f);
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
}

void PlayerActor::OnLateUpdate()
{
	if (!state.expired())
		state.lock()->OnLateUpdate(this);
}

void PlayerActor::Draw()
{
}

void PlayerActor::OnCollisionStay(std::weak_ptr<Collision>& mine, std::weak_ptr<Collision>& other)
{
}

void PlayerActor::OnTriggerStay(std::weak_ptr<Collision>& mine, std::weak_ptr<Collision>& other)
{
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
		Singleton<AudioClipManager>::Instance()->Play(AudioData::SE_SwordPachin);

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
		Singleton<AudioClipManager>::Instance()->Play(AudioData::SE_SwordPachin);

		isWeaponHold = false;

		if (!sword_HandContorller.expired()) sword_HandContorller.lock()->SetWeight(-0.1f);
		if (!handSword.expired()) handSword.lock()->SetActive(isWeaponHold);
		if (!backSword.expired()) backSword.lock()->SetActive(!isWeaponHold);

		if (!shield_HandContorller.expired()) shield_HandContorller.lock()->SetWeight(-0.1f);
		if (!handShield.expired()) handShield.lock()->SetActive(isWeaponHold);
		if (!backShield.expired()) backShield.lock()->SetActive(!isWeaponHold);
	});
}

void PlayerActor::RockOn(bool flag)
{
	isRockOn = flag;

	animator.lock()->SetBool("IsRockOn", isRockOn);

	// ロックオン時
	if (flag)
	{
		Singleton<AudioClipManager>::Instance()->Play(AudioData::SE_RockOn);
		// ロックオンする一番近い対象を探す
		for (auto & t : targetTriggerList)
		{
			if (targetTransform.expired())
			{
				targetTransform = t.lock()->transform;
				continue;
			}
			float dist1 = (targetTransform.lock()->GetWorldPosition() - transform.lock()->GetWorldPosition()).LengthSq();
			float dist2 = (t.lock()->transform.lock()->GetWorldPosition() - transform.lock()->GetWorldPosition()).LengthSq();
			if (dist1 > dist2)
			{
				targetTransform = t.lock()->transform;
			}
		}

		cameraController.lock()->ChangePlugin(CameraController::Plugin::RockOn);
	}
	// ロックオン解除
	else
	{
		Singleton<AudioClipManager>::Instance()->Play(AudioData::SE_RockOnChancel);
		targetTransform.reset();
		cameraController.lock()->ChangePlugin(CameraController::Plugin::Character);
	}
}

void PlayerActor::SetNextTarget(bool isRight)
{
	std::weak_ptr<Transform> nextTarget;
	Vector3 p_to_target = targetTransform.lock()->GetWorldPosition() - transform.lock()->GetWorldPosition();
	p_to_target.Normalize();

	Vector3 cross = Vector3::Cross(Vector3::up(), p_to_target);
	cross.Normalize();
	cross = (isRight) ? cross : -cross;

	// ロックオンする一番近い対象を探す
	for (auto & t : targetTriggerList)
	{
		if (t.lock()->transform.lock() == targetTransform.lock()) continue;

		Vector3 p_to_t = t.lock()->transform.lock()->GetWorldPosition() - transform.lock()->GetWorldPosition();
		p_to_t.Normalize();

		// プレイヤーより右か左かを判断
		float dot = Vector3::Dot(cross, p_to_t);
		if (dot > 0)
		{
			if (nextTarget.expired())
			{
				nextTarget = t.lock()->transform;
			}
			else
			{
				// 前に入ったターゲットより距離が近ければ採用
				float p_to_t_Length = (t.lock()->transform.lock()->GetWorldPosition() - targetTransform.lock()->GetWorldPosition()).LengthSq();
				float p_to_next_len = (nextTarget.lock()->GetWorldPosition() - targetTransform.lock()->GetWorldPosition()).LengthSq();
				if (p_to_t_Length < p_to_next_len)
				{
					nextTarget = t.lock()->transform;
				}
			}
		}
	}

	// 最終的に条件を満たしたターゲットに切り替え
	if (!nextTarget.expired())
	{
		Singleton<AudioClipManager>::Instance()->Play(AudioData::SE_RockOn);
		targetTransform = nextTarget;
		if (!targetImageController.expired())
			targetImageController.lock()->gameObject.lock()->SetActive(false);	// ターゲット画像のアニメーションを再スタート
	}
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
		
		auto effect = Singleton<GameObjectManager>::Instance()->Instantiate(Singleton<SceneManager>::Instance()->GetCurrentScene()->GetPrefabGameObject("Effect"));
		if (!effect.expired())
		{
			effect.lock()->transform.lock()->SetWorldPosition(hitInfo.point);
			//effect.lock()->GetComponent<EffekseerSystem>().lock();
		}

		Singleton<AudioClipManager>::Instance()->Play(AudioData::SE_Hit01);

		GameObject::Destroy(other.lock()->gameObject, 0.5f);
	}
	// 地面は判定しない
	else if (Vector3::Dot(Vector3::up(), hitInfo.normal) < 0.4f)
	{
		Vector3 normal = hitInfo.normal;
		normal.y = 0.0f;
		normal.Normalize();
		rigidbody.lock()->AddForce(normal * 1000.0f);

		auto effect = Singleton<GameObjectManager>::Instance()->Instantiate(
			Singleton<SceneManager>::Instance()->GetCurrentScene()->GetPrefabGameObject("Effect"));
		if (!effect.expired())
		{
			effect.lock()->transform.lock()->SetWorldPosition(hitInfo.point);
			//effect.lock()->GetComponent<EffekseerSystem>().lock();
		}

		Singleton<AudioClipManager>::Instance()->Play(AudioData::SE_Hit01);
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
