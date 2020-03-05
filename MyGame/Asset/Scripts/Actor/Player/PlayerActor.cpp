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
#include "State/PlayerAttackFlip.h"
#include "State/PlayerStep.h"
#include "State/PlayerDamage.h"
#include "State/PlayerGuard.h"

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

	ImGui::Text("Head Collision");
	MyImGui::DropTargetComponent(headCollision, strId);

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

void PlayerActor::Start()
{
	if (fsmManager) return;

	// 全てのステートの初期化
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
	fsmManager->AddState((int)State::AttackFlip, new PlayerAttackFlip());
	fsmManager->AddState((int)State::Step, new PlayerStep());
	fsmManager->AddState((int)State::Damage, new PlayerDamage());
	fsmManager->AddState((int)State::Guard, new PlayerGuard());

	// 入力の数分生成
	inputHandler.resize((int)InputKey::MaxNum);

	// 必要なコンポーネントを取得
	animator = this->gameObject.lock()->GetComponent<Animator>();
	rigidbody = this->gameObject.lock()->GetComponent<Rigidbody>();

	// ダメージ用のカラーの初期化
	damageFlashColor.SetRenderer(this->gameObject.lock()->GetComponent<Renderer>());

	// 初期姿勢に設定
	ChangeState(State::Idle);

	// 剣の当たり判定の設定
	if (!this->locusController.expired())
	{
		this->locusController.lock()->SetCollision(this->rigidbody, 
			[=](MeshCastInfo & hitInfo, MeshPoints& locusPoints, float locusLen)
		{
			AttackSwordHit(hitInfo, locusPoints, locusLen);
		});
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

	// 足音用の関数
	auto GroundSE = [=](std::string name, int frame)
	{
		animator.lock()->SetAnimationCallBack(name, frame, [=]()
		{
			// マテリアル情報から音を変える
			if (!castGroundInfo.material.expired())
			{
				switch (castGroundInfo.material.lock()->type)
				{
				case MaterialType::Soil:
					Singleton<AudioClipManager>::Instance()->Play(AudioData::SE_Foot_Soil);
					break;
				case MaterialType::Wood:
					Singleton<AudioClipManager>::Instance()->Play(AudioData::SE_Foot_Wood);
					break;
				case MaterialType::Stone:
					Singleton<AudioClipManager>::Instance()->Play(AudioData::SE_Foot_Stone);

					break;
				case MaterialType::Iron:
					Singleton<AudioClipManager>::Instance()->Play(AudioData::SE_Foot_Iron);

					break;
				}
			}
			Singleton<AudioClipManager>::Instance()->Play(AudioData::SE_SwordChakin);
		});
		
	};

	// アニメーションに応じてのSE再生用
	GroundSE("Walk", 8);
	GroundSE("Walk", 23);
	GroundSE("Run", 8);
	GroundSE("Run", 23);
	GroundSE("RockOn_Left_Walk", 7);
	GroundSE("RockOn_Left_Walk", 15);
	GroundSE("RockOn_Right_Walk", 7);
	GroundSE("RockOn_Right_Walk", 15);
	GroundSE("RockOn_Back_Walk", 7);
	GroundSE("RockOn_Back_Walk", 15);

	GroundSE("RockOn_Left_Run", 7);
	GroundSE("RockOn_Left_Run", 15);
	GroundSE("RockOn_Right_Run", 7);
	GroundSE("RockOn_Right_Run", 15);
	GroundSE("RockOn_Back_Run", 7);
	GroundSE("RockOn_Back_Run", 15);
}

void PlayerActor::Update()
{
	SetInput(InputKey::A_Trigger, Input::Keyboad::IsTrigger('E') || GamePad::IsTrigger(GamePad::Button::A));
	SetInput(InputKey::B_Trigger, Input::Keyboad::IsTrigger('R') || GamePad::IsTrigger(GamePad::Button::B));
	SetInput(InputKey::LS_Trigger, Input::Keyboad::IsTrigger('F') || GamePad::IsTrigger(GamePad::Button::LEFT_SHOULDER));
	SetInput(InputKey::LS_Press, Input::Keyboad::IsPress('F') || GamePad::IsPress(GamePad::Button::LEFT_SHOULDER));
	SetInput(InputKey::RS_Press, Input::Keyboad::IsPress('Q') || GamePad::IsPress(GamePad::Button::RIGHT_SHOULDER));
	SetInput(InputKey::Left_Trigger, Input::Keyboad::IsTrigger(VK_LEFT) || GamePad::IsTrigger(GamePad::Button::THUMB_R_LEFT));
	SetInput(InputKey::Right_Trigger, Input::Keyboad::IsTrigger(VK_RIGHT) || GamePad::IsTrigger(GamePad::Button::THUMB_R_RIGHT));

	// リストにある対象が既に死んでいないかチェック
	for (auto itr = targetTriggerList.begin(), end = targetTriggerList.end(); itr != end;)
	{
		if (itr->expired()) itr = targetTriggerList.erase(itr);
		else ++itr;
	}

	// ロックオン処理
	if (GetInput(InputKey::LS_Trigger))
	{
		RockOn(!isRockOn);
	}
	if (isRockOn)
	{
		//  ターゲットがいない場合は長押ししていないとロックオン状態を解除する
		if (targetTransform.expired())
		{
			if (!GetInput(InputKey::LS_Press))
				RockOn(false);
		}
		// ターゲット切り替え
		else if (GetInput(InputKey::Left_Trigger))
		{
			SetNextTarget(false);
		}
		else if (GetInput(InputKey::Right_Trigger))
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
				if (GetInput(InputKey::RS_Press))
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

	damageFlashColor.OnUpdate();
}

void PlayerActor::LateUpdate()
{
	if (!state.expired())
		state.lock()->OnLateUpdate(this);
}

DamageType PlayerActor::Damage(int damage, std::weak_ptr<Collision>& mine, std::weak_ptr<Collision>& other, Vector3 force)
{
	// ダメージ中と無敵時間以外
	if ((currentState == State::Damage || damageFlashColor.IsFlash()))
		return DamageType::None;

	RayCastInfo info;
	Ray ray(mine.lock()->worldMatrix.position(), other.lock()->worldMatrix.position());
	Vector3 toDir = ray.end - ray.start;
	// レイを飛ばしてどこに当たったのかを確かめる
	if (RayCast::JudgeCollision(&ray, &info, other))
	{
		auto effect = Singleton<GameObjectManager>::Instance()->Instantiate(Singleton<SceneManager>::Instance()->GetCurrentScene()->GetPrefabGameObject("Effect"));
		if (!effect.expired())
		{
			effect.lock()->transform.lock()->SetWorldPosition(info.point);
			effect.lock()->GetComponent<EffekseerSystem>().lock()->type = EffekseerType::Hit01;
			effect.lock()->GetComponent<EffekseerSystem>().lock()->Play();
		}

		Singleton<AudioClipManager>::Instance()->Play(AudioData::SE_Hit01);

		// 後方ダメージ（ 前からの衝突 ）
		if (Vector3::Dot(transform.lock()->forward(), toDir.Normalize()) > 0)
		{
			// ガード時
			if (!shieldRock_HandContorller.expired()
				&& shieldRock_HandContorller.lock()->IsWeightDone())
			{
				ChangeState(State::Guard);
				rigidbody.lock()->AddForce((-transform.lock()->forward() * 12.0f));
				return DamageType::Guard;
			}
			else
			{
				ChangeState(State::Damage);
				fsmManager->GetState<PlayerDamage>().lock()->SetDamage(this, PlayerDamage::DamageDirection::Back);
			}
		}
		// 前方ダメージ
		else
		{
			ChangeState(State::Damage);
			fsmManager->GetState<PlayerDamage>().lock()->SetDamage(this, PlayerDamage::DamageDirection::Forward);
		}

		return DamageType::Hit;
	}
	return DamageType::None;
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

void PlayerActor::SetInput(InputKey key, bool flag)
{
	inputHandler[(int)key] = flag;
}

bool PlayerActor::GetInput(InputKey key)
{
	return inputHandler[(int)key];
}

void PlayerActor::AttackSwordHit(MeshCastInfo & hitInfo, MeshPoints& locusPoints, float locusLengh)
{
	if (hitInfo.collision.lock()->isTrigger) return;

	auto other = hitInfo.collision.lock()->gameObject.lock()->GetComponent<BaseActor>();
	if(other.expired()) other = hitInfo.collision.lock()->gameObject.lock()->GetComponentWithParent<BaseActor>();

	// 他のアクターにヒット
	if (!other.expired())
	{
		int damage = 1;
		if (currentState == State::AttackJump) damage = 2;

		auto dummy = std::weak_ptr<Collision>();
		if (rigidbody.lock()->collisions.size() > 0) dummy = rigidbody.lock()->collisions.front();
		Vector3 damageDir = hitInfo.collision.lock()->transform.lock()->GetWorldPosition() - transform.lock()->GetWorldPosition();
		DamageType damageType = other.lock()->Damage(damage, hitInfo.collision, dummy, damageDir.Normalized() * 7.0f);

		if (damageType == DamageType::None) return;

		RayCastInfo info;
		Vector3 start = (locusPoints.point[0] + locusPoints.point[2]) * 0.5f;
		Vector3 dir = (locusPoints.point[3] - locusPoints.point[2]).Normalized();
		Ray ray(start, dir, locusLengh);
		DebugLine::DrawRay(ray.start, ray.end, Color::red());
		// レイを飛ばしてどこに当たったのかを確かめる
		if (RayCast::JudgeCollision(&ray, &info, hitInfo.collision))
		{
			hitInfo.point = info.point;
		}
		else // 当たっていなかったら違う方向から飛ばして確かめる
		{
			start = locusPoints.point[2];
			ray.Set(start, dir, locusLengh);
			DebugLine::DrawRay(ray.start, ray.end, Color::red());
			if (RayCast::JudgeCollision(&ray, &info, hitInfo.collision))
			{
				hitInfo.point = info.point;
			}
		}

		if (damageType == DamageType::Hit || damageType == DamageType::Deth)
		{
			auto effect = Singleton<GameObjectManager>::Instance()->Instantiate(Singleton<SceneManager>::Instance()->GetCurrentScene()->GetPrefabGameObject("Effect"));
			if (!effect.expired())
			{
				Vector3 pos = info.point + info.normal * 0.1f;
				effect.lock()->transform.lock()->SetWorldPosition(pos);
				Quaternion look = Quaternion::LookRotation(cameraController.lock()->cameraTransform.lock()->GetWorldPosition() - pos);
				effect.lock()->transform.lock()->SetWorldPosition(hitInfo.point);
				effect.lock()->GetComponent<EffekseerSystem>().lock()->type = EffekseerType::Hit01;
				effect.lock()->GetComponent<EffekseerSystem>().lock()->Play();
			}

			Singleton<AudioClipManager>::Instance()->Play(AudioData::SE_Hit01);

			int hitStop = 8;
			if (damageType == DamageType::Deth)
			{
				hitStop = 20;
			}

			// ヒットストップ
			this->gameObject.lock()->SetStop(true);
			other.lock()->gameObject.lock()->SetStop(true);
			locusController.lock()->gameObject.lock()->SetStop(true);
			FrameTimer::SetFuncTimer(hitStop, [=]()
			{
				this->gameObject.lock()->SetStop(false);
				other.lock()->gameObject.lock()->SetStop(false);
				locusController.lock()->gameObject.lock()->SetStop(false);
			});
		}
		else if(damageType == DamageType::Guard)
		{
			auto effect = Singleton<GameObjectManager>::Instance()->Instantiate(Singleton<SceneManager>::Instance()->GetCurrentScene()->GetPrefabGameObject("Effect"));
			if (!effect.expired())
			{
				Vector3 pos = info.point + info.normal * 0.1f;
				effect.lock()->transform.lock()->SetWorldPosition(pos);
				Quaternion look = Quaternion::LookRotation(cameraController.lock()->cameraTransform.lock()->GetWorldPosition() - pos);
				effect.lock()->transform.lock()->SetWorldPosition(hitInfo.point);
				effect.lock()->GetComponent<EffekseerSystem>().lock()->type = EffekseerType::Hit01;
				effect.lock()->GetComponent<EffekseerSystem>().lock()->Play();
			}

			Singleton<AudioClipManager>::Instance()->Play(AudioData::SE_Hit01);
		}
	}
	// 地面は判定しない
	else if (Vector3::Dot(Vector3::up(), hitInfo.normal) < 0.4f)
	{
		auto EffectSet = [&](RayCastInfo & info)
		{
			EffekseerType efcType = EffekseerType::Hit01;
			// マテリアル情報から音を変える
			if (!info.material.expired())
			{
				switch (info.material.lock()->type)
				{
				case MaterialType::Soil:
					efcType = EffekseerType::Hit01;
					Singleton<AudioClipManager>::Instance()->Play(AudioData::SE_Hit01);
					break;
				case MaterialType::Wood:
					efcType = EffekseerType::Hit01;
					Singleton<AudioClipManager>::Instance()->Play(AudioData::SE_Hit_Wood);
					break;
				case MaterialType::Stone:
					efcType = EffekseerType::Hit02;
					Singleton<AudioClipManager>::Instance()->Play(AudioData::SE_Hit_Stone);

					if(currentState == State::Attack)
						ChangeState(State::AttackFlip);
					break;
				case MaterialType::Iron:
					efcType = EffekseerType::Hit02;
					Singleton<AudioClipManager>::Instance()->Play(AudioData::SE_Hit_Iron);

					if (currentState == State::Attack)
					{
						ChangeState(State::AttackFlip);
					}
					break;
				} 
			}

			Vector3 normal = info.normal;
			normal.y = 0.0f;
			normal.Normalize();
			rigidbody.lock()->AddForce(normal * 1000.0f);

			// エフェクト生成
			auto effect = Singleton<GameObjectManager>::Instance()->Instantiate(
				Singleton<SceneManager>::Instance()->GetCurrentScene()->GetPrefabGameObject("Effect"));
			if (!effect.expired())
			{
				Vector3 pos = info.point + info.normal * 0.1f;
				effect.lock()->transform.lock()->SetWorldPosition(pos);
				Quaternion look = Quaternion::LookRotation(cameraController.lock()->cameraTransform.lock()->GetWorldPosition() - pos);
				effect.lock()->transform.lock()->SetWorldRotation(look);
				effect.lock()->GetComponent<EffekseerSystem>().lock()->type = efcType;
				effect.lock()->GetComponent<EffekseerSystem>().lock()->Play();
			}
		};

		RayCastInfo info;
		Vector3 start = (locusPoints.point[0] + locusPoints.point[2]) * 0.5f;
		Vector3 dir = (locusPoints.point[3] - locusPoints.point[2]).Normalized();
		start -= dir * (locusLengh * 0.5f);
		float rayLen = locusLengh * 1.5f;
		Ray ray(start, dir, rayLen);
		// レイを飛ばしてどこに当たったのかを確かめる
		if (RayCast::JudgeCollision(&ray, &info, hitInfo.collision))
		{
			// エフェクト生成
			EffectSet(info);
		}
		else // 当たっていなかったら違う方向から飛ばして確かめる
		{
			start = locusPoints.point[2] - dir * (locusLengh * 0.5f);
			ray.Set(start, dir, rayLen);
			DebugLine::DrawRay(ray.start, ray.end, Color::red());
			if (RayCast::JudgeCollision(&ray, &info, hitInfo.collision))
			{
				// エフェクト生成
				EffectSet(info);
			}
		}
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

	if (RayCast::JudgeAllCollision(&downRay, &castGroundInfo, rigidbody.lock()->collisions))
	{
		auto other = castGroundInfo.collision.lock()->gameObject.lock()->GetComponent<BaseActor>();
		if (other.expired()) other = castGroundInfo.collision.lock()->gameObject.lock()->GetComponentWithParent<BaseActor>();

		if (other.expired())	// アクターは対象外
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
	if (RayCast::JudgeAllCollision(&ray, &castCliffWallInfo, rigidbody.lock()->collisions))
	{
		auto other = castCliffWallInfo.collision.lock()->gameObject.lock()->GetComponent<BaseActor>();
		if (!other.expired())	// アクターは対象外
		{
			castCliffWallInfo.collision.reset();
		}
	}
	// 前方の下に飛ばすレイ
	ray.Set(pos + Vector3(0.0f, cliffRayStartY_Down, 0.0f) + transform.lock()->forward() * cliffRayStartZ_Down, Vector3::down(), cliffRayStartY_Down - cliffRayStartY_Front);
	DebugLine::DrawRay(ray.start, ray.end, Color::red());
	if (RayCast::JudgeAllCollision(&ray, &castCliffGroundInfo, rigidbody.lock()->collisions))
	{
		auto other = castCliffGroundInfo.collision.lock()->gameObject.lock()->GetComponent<BaseActor>();
		if (!other.expired())	// アクターは対象外
		{
			castCliffGroundInfo.collision.reset();
		}
	}
}
