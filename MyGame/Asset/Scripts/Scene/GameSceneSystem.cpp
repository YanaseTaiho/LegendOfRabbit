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
}

void GameSceneSystem::Start()
{
	player.lock()->OnStart();

	cameraController.lock()->OnStart();
	cameraController.lock()->ChangePlugin(CameraController::Plugin::Character);

	/*clip = new CAudioClip();
	clip->Load("Asset/Art/Sound/Battle2.wav");
	clip->Play(true);*/
}

void GameSceneSystem::Update()
{
	player.lock()->OnUpdate();
	cameraController.lock()->OnUpdate();

	/*if (Input::Keyboad::IsTrigger(VK_RETURN))
	{
		SceneManager::Instance()->SceneChange(new TitleScene());
	}*/
}

void GameSceneSystem::LateUpdate()
{
	player.lock()->OnLateUpdate();
	cameraController.lock()->OnLateUpdate();
}

void GameSceneSystem::OnDestroy()
{
	/*clip->Stop();
	clip->Unload();
	delete clip;*/
}
