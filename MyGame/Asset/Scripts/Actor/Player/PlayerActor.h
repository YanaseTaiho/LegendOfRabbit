#ifndef _PLAYERACTOR_H_
#define _PLAYERACTOR_H_

#include "../BaseActor.h"
#include "../FSMManager.h"

class RotationFixedController;

class PlayerActor : public BaseActor
{
private:
	friend cereal::access;
	template<class Archive>
	void save(Archive & archive, std::uint32_t const version) const
	{
		archive(cereal::make_nvp("MonoBehaviour", cereal::base_class<MonoBehaviour>(this)));
		archive(cameraTransform);

		if (version >= 1)
			archive(rayStart, rayLength, height, jumpForce, moveForce, forceMax);

		if (version >= 2)
			archive(cliffJumpForce, cliffRayStartY_Front, cliffRayLength_Front, cliffRayStartY_Down, cliffRayStartZ_Down);

		if (version >= 3)
			archive(sorwd_HandContorller, shield_HandContorller, shieldRock_HandContorller, backSword, backShield, handSword, handShield);
	}

	template<class Archive>
	void load(Archive & archive, std::uint32_t const version)
	{
		archive(cereal::make_nvp("MonoBehaviour", cereal::base_class<MonoBehaviour>(this)));
		archive(cameraTransform);

		if(version >= 1)
			archive(rayStart, rayLength, height, jumpForce, moveForce, forceMax);

		if (version >= 2)
			archive(cliffJumpForce, cliffRayStartY_Front, cliffRayLength_Front, cliffRayStartY_Down, cliffRayStartZ_Down);

		if (version >= 3)
			archive(sorwd_HandContorller, shield_HandContorller, shieldRock_HandContorller, backSword, backShield, handSword, handShield);
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
		Attack
	};

	enum class AttackType : int
	{
		Inside,
		Outside,
		Upper,
		Thrust,
		Jump
	};

	std::weak_ptr<Transform> cameraTransform;

	std::weak_ptr<RotationFixedController> sorwd_HandContorller;	// 剣装備時
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

	bool onGround = false;

	void OnStart() override;
	void OnUpdate() override;
	void OnLateUpdate() override;
	void Draw() override;

	void ChangeState(State state);

private:
	float horizontal = 0.0f;
	float vertical = 0.0f;
	
	void UpdateInput();
	void CheckGround();
	void CheckCliff();
};

CEREAL_CLASS_VERSION(PlayerActor, 3)
CEREAL_REGISTER_TYPE(PlayerActor)

#endif // !_PLAYERACTOR_H_
