#ifndef _SMALL_GOMA_ACTOR_H_
#define _SMALL_GOMA_ACTOR_H_

#include "../../BaseActor.h"
#include "../../FSMManager.h"

class PlayerActor;

class SmallGomaActor : public BaseActor
{
private:
	friend cereal::access;
	template<class Archive>
	void save(Archive & archive, std::uint32_t const version) const
	{
		archive(cereal::base_class<MonoBehaviour>(this));
		archive(moveSpeed);
	}

	template<class Archive>
	void load(Archive & archive, std::uint32_t const version)
	{
		archive(cereal::base_class<MonoBehaviour>(this));
		archive(moveSpeed);
	}

	void DrawImGui(int id) override;

public:
	SmallGomaActor() {}
	~SmallGomaActor() {}

	enum class State : int
	{
		Idle,
		Move,
		Chase,
		Attack,
	};

	State currentState;
	std::unique_ptr<FSMManager<SmallGomaActor>> fsmManager;
	std::weak_ptr<BaseFSM<SmallGomaActor>> state;
	std::weak_ptr<Animator> animator;
	std::weak_ptr<Rigidbody> rigidbody;

	float moveSpeed = 10.0f;

	std::weak_ptr<PlayerActor> player;

	void OnStart() override;
	void OnUpdate() override;
	void OnLateUpdate() override;

	void ChangeState(State state);

	// ï®óùìIÇ…ìñÇΩÇ¡ÇƒÇ¢ÇÈä‘ÇÕåƒÇŒÇÍÇÈ
	void OnCollisionStay(std::weak_ptr<Collision> & mine, std::weak_ptr<Collision> & other) override;
	void OnTriggerStay(std::weak_ptr<Collision> & mine, std::weak_ptr<Collision> & other) override;

private:

	//void CheckGround();
};

CEREAL_CLASS_VERSION(SmallGomaActor, 0)
CEREAL_REGISTER_TYPE(SmallGomaActor)

#endif // !_SMALL_GOMA_ACTOR_H_
