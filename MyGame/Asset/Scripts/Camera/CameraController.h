#ifndef _CAMERACONTROLLER_H_
#define _CAMERACONTROLLER_H_

#include "FrameWork/Common.h"
#include "CameraPlugin.h"

class PlayerActor;

class CameraController : public MonoBehaviour
{
private:
	friend cereal::access;
	template<class Archive>
	void save(Archive & archive, std::uint32_t const version) const
	{
		archive(cereal::make_nvp("MonoBehaviour", cereal::base_class<MonoBehaviour>(this)));
		archive(cameraTransform, targetTransform, playerActor);
		archive(targetDistance, distanceSpeed, offsetHeight);
	}

	template<class Archive>
	void load(Archive & archive, std::uint32_t const version)
	{
		archive(cereal::make_nvp("MonoBehaviour", cereal::base_class<MonoBehaviour>(this)));
		archive(cameraTransform, targetTransform, playerActor);
		archive(targetDistance, distanceSpeed, offsetHeight);
	}

	void DrawImGui(int id) override;

public:

	enum class Plugin : int
	{
		Character,
		RockOn
	};

	void OnStart();
	void OnUpdate();
	void OnLateUpdate();

	void AddPlugin(Plugin key, CameraPlugin * plugin);
	void ChangePlugin(Plugin key);

	std::weak_ptr<Transform> cameraTransform;	// カメラのトランスフォーム
	std::weak_ptr<Transform> targetTransform;	// カメラの注視点のトランスフォーム
	std::weak_ptr<PlayerActor> playerActor;		// プレイヤーコンポーネント

	float targetDistance = 30.0f;
	float distanceSpeed = 1.0f;
	float offsetHeight = 4.0f;
private:

	std::unordered_map<Plugin,std::shared_ptr<CameraPlugin>> pluginMap;
	std::weak_ptr<CameraPlugin> currentPlugin;

	bool CollisionCheck();
	void UpdateDistance(float distance, float speed);
};

CEREAL_CLASS_VERSION(CameraController, 0)
CEREAL_REGISTER_TYPE(CameraController)

#endif // !_CAMERACONTROLLER_H_

