#ifndef _PLAYERACTOR_H_
#define _PLAYERACTOR_H_

#include "../BaseActor.h"
#include "../FSMManager.h"

class RotationFixedController;
class CameraController;
class LocusController;

class PlayerActor : public BaseActor
{
private:
	friend cereal::access;
	template<class Archive>
	void save(Archive & archive, std::uint32_t const version) const
	{
		archive(cereal::make_nvp("MonoBehaviour", cereal::base_class<MonoBehaviour>(this)));

		archive(cameraController);
		archive(rayStart, rayLength, height, jumpForce, moveForce, forceMax);
		archive(cliffJumpForce, cliffRayStartY_Front, cliffRayLength_Front, cliffRayStartY_Down, cliffRayStartZ_Down);
		archive(sword_HandContorller, shield_HandContorller, shieldRock_HandContorller, backSword, backShield, handSword, handShield);

		if (version >= 1)
			archive(locusController);
	}

	template<class Archive>
	void load(Archive & archive, std::uint32_t const version)
	{
		archive(cereal::make_nvp("MonoBehaviour", cereal::base_class<MonoBehaviour>(this)));

		archive(cameraController);
		archive(rayStart, rayLength, height, jumpForce, moveForce, forceMax);
		archive(cliffJumpForce, cliffRayStartY_Front, cliffRayLength_Front, cliffRayStartY_Down, cliffRayStartZ_Down);
		archive(sword_HandContorller, shield_HandContorller, shieldRock_HandContorller, backSword, backShield, handSword, handShield);

		if (version >= 1)
			archive(locusController);
	}

	void DrawImGui(int id) override;

public:
	PlayerActor();
	~PlayerActor();

	enum class State : int
	{
		Idle,
		Move,
		Air,
		Roll,
		RollStop,
		CliffGrap,
		CliffJump,
		AttackJump,
		Attack,
		Step
	};

	enum class AttackType : int
	{
		Inside,
		Outside,
		Upper,
		Thrust,
		Jump
	};

	enum class Direction : int
	{
		Forward,
		Left,
		Right,
		Back,
	};

	std::weak_ptr<CameraController> cameraController;

	std::weak_ptr<RotationFixedController> sword_HandContorller;	// 剣装備時
	std::weak_ptr<RotationFixedController> shield_HandContorller;	// 盾装備時
	std::weak_ptr<RotationFixedController> shieldRock_HandContorller;// 盾を構えている時

	std::weak_ptr<GameObject> backSword;
	std::weak_ptr<GameObject> backShield;
	std::weak_ptr<GameObject> handSword;
	std::weak_ptr<GameObject> handShield;

	std::unique_ptr<FSMManager<PlayerActor>> fsmManager;
	std::weak_ptr<BaseFSM<PlayerActor>> state;
	std::weak_ptr<Animator> animator;
	std::weak_ptr<Rigidbody> rigidbody;

	State currentState;
	RayCastInfo castGroundInfo;
	RayCastInfo castCliffWallInfo;	// 掴める崖の壁のデータ
	RayCastInfo castCliffGroundInfo;	// 掴める崖の地面のデータ

	// ソードの軌跡コントローラー
	std::weak_ptr<LocusController> locusController;

	float rayStart = 5.0f;
	float rayLength = 10.0f;
	float height = 0.0f;
	float jumpForce = 1.0f;
	float moveForce = 3.0f;
	float forceMax = 5.0f;
	float forceAmount = 0.0f;
	float horizontalRegistance = 0.2f;

	float cliffJumpForce = 1.0f;
	float cliffRayStartY_Front = 5.0f;
	float cliffRayLength_Front = 10.0f;
	float cliffRayStartY_Down = 0.0f;
	float cliffRayStartZ_Down = 0.0f;

	// 移動処理用変数
	Vector3 moveDir;
	float moveAmount = 0.0f;

	bool isWeaponHold = false; // 装備品を手に持っているか
	bool onGround = false;
	bool isRockOn = false;

	void OnStart() override;
	void OnUpdate() override;
	void OnLateUpdate() override;
	void Draw() override;

	void ChangeState(State state);

	void WeaponHold(std::function<void()> func = nullptr);
	void WeaponNotHold();

private:
	float horizontal = 0.0f;
	float vertical = 0.0f;
	
	void AttackSwordHit(MeshCastInfo hitInfo);
	void UpdateInput();
	void CheckGround();
	void CheckCliff();
};

CEREAL_CLASS_VERSION(PlayerActor, 1)
CEREAL_REGISTER_TYPE(PlayerActor)

#endif // !_PLAYERACTOR_H_
