#ifndef _PLAYERCONTROLLER_H_
#define _PLAYERCONTROLLER_H_

#include "PlayerActor.h"

class PlayerController
{
//private:
//	friend cereal::access;
//	template<class Archive>
//	void serialize(Archive & archive, std::uint32_t const version)
//	{
//		archive(player);
//	}
public:
	PlayerController();
	~PlayerController();

	void OnStart();
	void OnUpdate();
	void OnLateUpdate();

	std::weak_ptr<PlayerActor> player;
};

//CEREAL_CLASS_VERSION(PlayerController, 0)

#endif // !_PLAYERCONTROLLER_H_
