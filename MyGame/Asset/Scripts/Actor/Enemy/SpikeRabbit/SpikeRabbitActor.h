#ifndef _SPIKE_RABBIT_ACTOR_H_
#define _SPIKE_RABBIT_ACTOR_H_

#include "../../BaseActor.h"
#include "../../FSMManager.h"

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
	}

	template<class Archive>
	void load(Archive & archive, std::uint32_t const version)
	{
		archive(cereal::base_class<MonoBehaviour>(this));
		archive(moveSpeed, attackMoveSpeed);
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

	float moveSpeed = 10.0f;
	float attackMoveSpeed = 20.0f;

	std::weak_ptr<PlayerActor> player;

	void OnStart() override;
	void OnUpdate() override;
	void OnLateUpdate() override;

	void ChangeState(State state);

	// ï®óùìIÇ…ìñÇΩÇ¡ÇƒÇ¢ÇÈä‘ÇÕåƒÇŒÇÍÇÈ
	void OnCollisionStay(std::weak_ptr<Collision> & mine, std::weak_ptr<Collision> & other) override;
	void OnTriggerStay(std::weak_ptr<Collision> & mine, std::weak_ptr<Collision> & other) override;

private:
	void OnDestroy() override;
	//void CheckGround();
};

CEREAL_CLASS_VERSION(SpikeRabbitActor, 0)
CEREAL_REGISTER_TYPE(SpikeRabbitActor)

#endif // !_SPIKE_RABBIT_ACTOR_H_
