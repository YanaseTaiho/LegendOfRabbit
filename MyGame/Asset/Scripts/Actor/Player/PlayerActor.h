#ifndef _PLAYERACTOR_H_
#define _PLAYERACTOR_H_

#include "../BaseActor.h"
#include "../FSMManager.h"
#include "../../FlashColor.h"

class RotationFixedController;
class CameraController;
class LocusController;
class TargetImageController;

using namespace Input;

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

		if (version >= 1) archive(locusController);
		if (version >= 2) archive(targetImageController);
		if (version >= 3) archive(headCollision);
	}

	template<class Archive>
	void load(Archive & archive, std::uint32_t const version)
	{
		archive(cereal::make_nvp("MonoBehaviour", cereal::base_class<MonoBehaviour>(this)));

		archive(cameraController);
		archive(rayStart, rayLength, height, jumpForce, moveForce, forceMax);
		archive(cliffJumpForce, cliffRayStartY_Front, cliffRayLength_Front, cliffRayStartY_Down, cliffRayStartZ_Down);
		archive(sword_HandContorller, shield_HandContorller, shieldRock_HandContorller, backSword, backShield, handSword, handShield);

		if (version >= 1) archive(locusController);
		if (version >= 2) archive(targetImageController);
		if (version >= 3) archive(headCollision);
	}

	void DrawImGui(int id) override;

public:
	PlayerActor();
	~PlayerActor();

	enum class InputKey : int
	{
		A_Trigger,
		B_Trigger,
		LS_Trigger,
		LS_Press,
		RS_Press,
		Left_Trigger,
		Right_Trigger,
		MaxNum
	};

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
		AttackFlip,
		Step,
		Damage
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
	std::weak_ptr<Collision> headCollision;

	std::weak_ptr<RotationFixedController> sword_HandContorller;	// ��������
	std::weak_ptr<RotationFixedController> shield_HandContorller;	// ��������
	std::weak_ptr<RotationFixedController> shieldRock_HandContorller;// �����\���Ă��鎞

	std::weak_ptr<GameObject> backSword;
	std::weak_ptr<GameObject> backShield;
	std::weak_ptr<GameObject> handSword;
	std::weak_ptr<GameObject> handShield;

	std::unique_ptr<FSMManager<PlayerActor>> fsmManager;
	std::weak_ptr<BaseFSM<PlayerActor>> state;
	std::weak_ptr<Animator> animator;
	std::weak_ptr<Rigidbody> rigidbody;
	std::list<std::weak_ptr<BaseActor>> targetTriggerList;	// �v���C���[�̃��b�N�I���Ώۂ̃��X�g
	FlashColor damageFlashColor;	// �_���[�W���󂯂����̓_�ŗp�i ���G���� �j

	State currentState;
	RayCastInfo castGroundInfo;
	RayCastInfo castCliffWallInfo;	// �͂߂�R�̕ǂ̃f�[�^
	RayCastInfo castCliffGroundInfo;	// �͂߂�R�̒n�ʂ̃f�[�^

	// �\�[�h�̋O�ՃR���g���[���[
	std::weak_ptr<LocusController> locusController;

	std::weak_ptr<Transform> targetTransform;

	std::weak_ptr<TargetImageController> targetImageController; // ���ڂ��Ă���^�[�Q�b�g�̈ʒu�Ɍ����摜

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

	// �ړ������p�ϐ�
	Vector3 moveDir;
	float moveAmount = 0.0f;

	bool isWeaponHold = false; // �����i����Ɏ����Ă��邩
	bool onGround = false;
	bool isRockOn = false;

	void OnStart() override;
	void OnUpdate() override;
	void OnLateUpdate() override;
	void Draw() override;

	// �����I�ɓ������Ă���Ԃ͌Ă΂��
	void OnCollisionStay(std::weak_ptr<Collision> & mine, std::weak_ptr<Collision> & other) override;
	void OnTriggerStay(std::weak_ptr<Collision> & mine, std::weak_ptr<Collision> & other) override;

	void ChangeState(State state);

	void WeaponHold(std::function<void()> func = nullptr);
	void WeaponNotHold();

	void RockOn(bool flag);
	void SetNextTarget(bool isRight);

	bool GetInput(InputKey key);

private:
	float horizontal = 0.0f;
	float vertical = 0.0f;
	
	std::vector<bool> inputHandler;
	void SetInput(InputKey key, bool flag);

	void AttackSwordHit(MeshCastInfo & hitInfo, MeshPoints& locusPoints, float locusLengh);
	void UpdateInput();
	void CheckGround();
	void CheckCliff();
};

CEREAL_CLASS_VERSION(PlayerActor, 3)
CEREAL_REGISTER_TYPE(PlayerActor)

#endif // !_PLAYERACTOR_H_
