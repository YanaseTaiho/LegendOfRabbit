#ifndef __$safeitemname$__H__
#define __$safeitemname$__H__

#include "../CameraPlugin.h"

class $safeitemname$ : public CameraPlugin
{
public:
	void OnStart(CameraController * controller) override;
	void OnUpdate(CameraController * controller) override;
	void OnLateUpdate(CameraController * controller) override;
private:
};

#endif // !__$safeitemname$__H__
