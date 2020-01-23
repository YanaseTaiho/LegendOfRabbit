#ifndef _BASEFSM_H_
#define _BASEFSM_H_

template<typename State>
class BaseFSM
{
public:
	BaseFSM() {}
	virtual ~BaseFSM() {}

	virtual void OnStart(State * actor) {}
	virtual void OnUpdate(State * actor) {}
	virtual void OnLateUpdate(State * actor) {}
	virtual void OnDestroy(State * actor) {}
};

#endif //!_BASEFSM_H_
