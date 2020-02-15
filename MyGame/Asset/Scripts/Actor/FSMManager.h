#ifndef _FSMMANAGER_H_
#define _FSMMANAGER_H_

#include "BaseFSM.h"
#include "BaseActor.h"
#include <unordered_map>
#include <memory>

template<typename State>
class FSMManager
{
public:
	void AddState(int key, BaseFSM<State> * fsm)
	{
		fsmMap[key].reset(fsm);
	}

	bool IsState(int key)
	{
		return fsmMap.count(key) > 0;
	}

	std::weak_ptr<BaseFSM<State>> GetState(int key)
	{
		if (fsmMap.count(key) == 0) return std::weak_ptr<BaseFSM<State>>();
		return fsmMap[key];
	}

	template <typename T>
	std::weak_ptr<T> GetState()
	{
		for (auto map : fsmMap)
		{
			if (typeid(*map.second) == typeid(T))
			{
				return std::dynamic_pointer_cast<T>(map.second);
			}
		}

		return std::weak_ptr<T>();
	}

	void ChangeState(State * actor, int key)
	{
		if (!actor->state.expired())
		{
			actor->state.lock()->OnDestroy(actor);
		}
		auto state = GetState(key);
		if (state.expired()) return;
		actor->state = state;
		actor->state.lock()->OnStart(actor);
	}
private:
	std::unordered_map<int, std::shared_ptr<BaseFSM<State>>> fsmMap;
};

#endif // !_FSMMANAGER_H_

