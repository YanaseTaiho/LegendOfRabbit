#include "GameScene.h"
#include "DirectX/Common.h"
#include "../Main/manager.h"

using namespace FrameWork;

void GameScene::Initialize()
{
	prefab.Load(DATA_FOLDER_PATH);

	Singleton<SceneManager>::Instance()->LoadSceneData("GameScene01");
}
