#ifndef _SPIKE_RABBIT_ACTOR_H_
#define _SPIKE_RABBIT_ACTOR_H_

#include "../../BaseActor.h"
#include "../../FSMManager.h"
#include "../../../FlashColor.h"

class PlayerActor;

class SpikeRabbitActor : public BaseActor
{
private:
	friend cereal::access;
	template<class Archive>
	void save(Archive & archive, std::uint32_t const version) const
	{
		archive(cereal::base_class<MonoBehaviour>(this));
		archive(moveSpeed, attackMoveSpeed);

		if (version >= 1) archive(rayLength);
		if (version >= 2) archive(attackCollision);
	}

	template<class Archive>
	void load(Archive & archive, std::uint32_t const version)
	{
		archive(cereal::base_class<MonoBehaviour>(this));
		archive(moveSpeed, attackMoveSpeed);

		if (version >= 1) archive(rayLength);
		if (version >= 2) archive(attackCollision);
	}

	void DrawImGui(int id) override;

public:
	SpikeRabbitActor() {}
	~SpikeRabbitActor() {}

	enum class State : int
	{
		Idle,
		Move,
		Attack_Ready,
		Attack,
		Attack_Stop,
		Attack_Finish,
		Damage,
		Deth,
	};

	State currentState;
	std::unique_ptr<FSMManager<SpikeRabbitActor>> fsmManager;
	std::weak_ptr<BaseFSM<SpikeRabbitActor>> state;
	std::weak_ptr<Animator> animator;
	std::weak_ptr<Rigidbody> rigidbody;

	std::weak_ptr<Collision> attackCollision;	// �U���͈͂̃R���W����

	float moveSpeed = 10.0f;
	float attackMoveSpeed = 20.0f;

	std::weak_ptr<PlayerActor> player;
	FlashColor damageFlashColor;	// �_���[�W���󂯂����̓_�ŗp�i ���G���� �j
	float playerFindTime;				// �v���C���[������������

	void Start() override;
	void Update() override;
	void LateUpdate() override;

	void ChangeState(State state);

	// �����I�ɓ������Ă���Ԃ͌Ă΂��
	void OnCollisionStay(std::weak_ptr<Collision> & mine, std::weak_ptr<Collision> & other) override;
	void OnTriggerStay(std::weak_ptr<Collision> & mine, std::weak_ptr<Collision> & other) override;

	DamageType Damage(int damage, std::weak_ptr<Collision>& mine, std::weak_ptr<Collision>& other, Vector3 force = Vector3::zero()) override;
private:
	float rayLength = 10.0f;
	int hitPoint;	// �̗�

	void OnDestroy() override;
	void CheckGround();
};

CEREAL_CLASS_VERSION(SpikeRabbitActor, 2)
CEREAL_REGISTER_TYPE(SpikeRabbitActor)

#endif // !_SPIKE_RABBIT_ACTOR_H_
