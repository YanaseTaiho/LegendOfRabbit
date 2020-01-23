#ifndef _GAMESCENESYSTEM_H_
#define _GAMESCENESYSTEM_H_

#include "FrameWork/Component/MonoBehaviour/MonoBehaviour.h"
#include "../Actor/Player/PlayerActor.h"
#include "../Camera/CameraController.h"

//#include "Main/audio_clip.h"

class GameSceneSystem : public MonoBehaviour
{
private:
	friend cereal::access;
	template<class Archive>
	void save(Archive & archive, std::uint32_t const version) const
	{
		archive(cereal::make_nvp("MonoBehaviour", cereal::base_class<MonoBehaviour>(this)));
		archive(CEREAL_NVP(player), CEREAL_NVP(cameraController));
	}

	template<class Archive>
	void load(Archive & archive, std::uint32_t const version)
	{
		archive(cereal::make_nvp("MonoBehaviour", cereal::base_class<MonoBehaviour>(this)));
		archive(CEREAL_NVP(player), CEREAL_NVP(cameraController));
	}

public:
	GameSceneSystem();
	~GameSceneSystem();

	void DrawImGui(int id) override;

	void Start() override;
	void Update() override;
	void LateUpdate() override;

	void OnDestroy() override;

private:
	std::weak_ptr<PlayerActor> player;
	std::weak_ptr<CameraController> cameraController;

	//CAudioClip * clip;
};

CEREAL_CLASS_VERSION(GameSceneSystem, 0)
CEREAL_REGISTER_TYPE(GameSceneSystem)

#endif // !_GAMESCENESYSTEM_H_