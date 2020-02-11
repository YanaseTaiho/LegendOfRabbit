#include "GameSceneSystem.h"

GameSceneSystem::GameSceneSystem()
{
}

GameSceneSystem::~GameSceneSystem()
{}

void GameSceneSystem::DrawImGui(int id)
{
	ImGui::Text("Player Actor"); //ImGui::SameLine();
	MyImGui::DropTargetComponent(player, std::to_string(id));
	ImGui::Text("Camera Controller"); //ImGui::SameLine();
	MyImGui::DropTargetComponent(cameraController, std::to_string(id));
	ImGui::Text("Enemy Container"); //ImGui::SameLine();
	MyImGui::DropTargetComponent(enemyContainer, std::to_string(id));
}

void GameSceneSystem::Start()
{
	player.lock()->cameraController = cameraController;
	cameraController.lock()->playerActor = player;

	player.lock()->OnStart();

	cameraController.lock()->OnStart();
	cameraController.lock()->ChangePlugin(CameraController::Plugin::Character);

	enemyContainer.lock()->OnStart();

	Singleton<AudioClipManager>::Instance()->Play(AudioData::BGM_StageBGM01, 1.0f, true);
}

void GameSceneSystem::Update()
{
	player.lock()->OnUpdate();
	cameraController.lock()->OnUpdate();
	enemyContainer.lock()->OnUpdate();
	/*if (Input::Keyboad::IsTrigger(VK_RETURN))
	{
		SceneManager::Instance()->SceneChange(new TitleScene());
	}*/
}

void GameSceneSystem::LateUpdate()
{
	player.lock()->OnLateUpdate();
	enemyContainer.lock()->OnLateUpdate();
	cameraController.lock()->OnLateUpdate();
}

void GameSceneSystem::OnDestroy()
{
	Singleton<AudioClipManager>::Instance()->Stop(AudioData::BGM_StageBGM01);
}
