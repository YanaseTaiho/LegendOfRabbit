#ifndef _CAMERAPLUGIN_H_
#define _CAMERAPLUGIN_H_

class CameraController;

class CameraPlugin
{
public:
	CameraPlugin() {};
	virtual ~CameraPlugin() {};

	virtual void OnStart(CameraController * controller) {};
	virtual void OnUpdate(CameraController * controller) {};
	virtual void OnLateUpdate(CameraController * controller) {};
	virtual void OnDestroy(CameraController * controller) {};
};

#endif // !_CAMERAPLUGIN_H_

