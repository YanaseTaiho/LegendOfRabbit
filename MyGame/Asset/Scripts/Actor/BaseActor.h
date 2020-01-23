#ifndef _BASEACTOR_H_
#define _BASEACTOR_H_

#include "FrameWork/Common.h"
#include "BaseFSM.h"

class BaseActor : public MonoBehaviour
{
public:
	BaseActor() {}
	virtual ~BaseActor() {}

	virtual void OnStart() {}
	virtual void OnUpdate() {}
	virtual void OnLateUpdate(){}

private:
	
};

#endif // !_BASEACTOR_H_

