#ifndef _ACTOR_CONTAINER__H__
#define _ACTOR_CONTAINER__H__

#include "../Actor/BaseActor.h"

class ActorContainer : public MonoBehaviour
{
private:
	friend cereal::access;
	template<class Archive>
	void save(Archive & archive, std::uint32_t const version) const
	{
		archive(cereal::base_class<MonoBehaviour>(this));
	}

	template<class Archive>
	void load(Archive & archive, std::uint32_t const version)
	{
		archive(cereal::base_class<MonoBehaviour>(this));
	}

public:
	ActorContainer() {}
	~ActorContainer() {}

	void DrawImGui(int id) override;

	void OnStart();

	std::list<std::weak_ptr<BaseActor>> actorList;
private:
};

CEREAL_CLASS_VERSION(ActorContainer, 0)
CEREAL_REGISTER_TYPE(ActorContainer)

#endif // ! _ACTOR_CONTAINER__H__