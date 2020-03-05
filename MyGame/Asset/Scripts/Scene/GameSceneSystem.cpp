#include "GameSceneSystem.h"
#include "../Camera/Plugin/CharacterCameraPlugin.h"
#include "../Camera/Plugin/RockOnCameraPlugin.h"
#include "../SceneChangePointContainer.h"

GameSceneSystem::GameSceneSystem()
{
}

GameSceneSystem::~GameSceneSystem()
{}

void GameSceneSystem::DrawImGui(int id)
{
	std::string strId = "##GameSceneSystem" + std::to_string(id);

	ImGui::Text("Player Actor"); //ImGui::SameLine();
	MyImGui::DropTargetComponent(player, std::to_string(id));
	ImGui::Text("Camera Controller"); //ImGui::SameLine();
	MyImGui::DropTargetComponent(cameraController, std::to_string(id));
	ImGui::Text("Enemy Container"); //ImGui::SameLine();
	MyImGui::DropTargetComponent(enemyContainer, std::to_string(id));

	if (ImGui::InputInt(("PlayerEntryNumber" + strId).c_str(), &playerEntryNumber))
	{
		playerEntryNumber = Mathf::Min(playerEntryNumber, 0);
	}
}

void GameSceneSystem::Start()
{
	this->isSceneChange = false;

	auto gameScene = Component::FindComponent<GameSceneSystem>();
	if (!gameScene.expired())
	{
		// ���Ɏ��g�Ƃ͈قȂ铯���N���X�����݂��Ă����玩�g���폜
		if (gameScene.lock()->gameObject.lock() != this->gameObject.lock())
		{
			GameObject::Destroy(this->gameObject);
			return;
		}
	}

	// �V�[����؂�ւ��Ă��폜����Ȃ��I�u�W�F�N�g�Ƃ��ēo�^
	GameObject::DontDestroyOnLoad(this->gameObject);

	player.lock()->cameraController = cameraController;
	cameraController.lock()->playerActor = player;

	cameraController.lock()->AddPlugin(CameraController::Plugin::Character, new CharacterCameraPlugin());
	cameraController.lock()->AddPlugin(CameraController::Plugin::RockOn, new RockOnCameraPlugin());
	cameraController.lock()->ChangePlugin(CameraController::Plugin::Character);

	// �G���g���[�|�C���g�̃R���e�i���擾
	auto entryContainer = Component::FindComponent<SceneChangePointContainer>();
	if (!entryContainer.expired())
	{
		// �G���g���[�ԍ�����v���C���[���X�^�[�g����ʒu�����߂�
		if (playerEntryNumber < entryContainer.lock()->pointArray.size()
			&& !entryContainer.lock()->pointArray[playerEntryNumber].expired())
		{
			auto entryTransform = entryContainer.lock()->pointArray[playerEntryNumber].lock()->transform.lock();
			Vector3 entryPos = entryTransform->GetWorldPosition() + entryTransform->forward() * 80.0f;
			Quaternion entryRot = entryTransform->GetWorldRotation();
			player.lock()->transform.lock()->SetWorldPosition(entryPos);
			player.lock()->transform.lock()->SetWorldRotation(entryRot);

			cameraController.lock()->transform.lock()->SetWorldPosition(entryPos);
			cameraController.lock()->transform.lock()->SetWorldRotation(entryRot);
		}

		for (auto point : entryContainer.lock()->pointArray)
		{
			if (point.expired()) continue;

			// �v���C���[���e�|�C���g�ɓ��B�����Ƃ��ɌĂ΂��
			point.lock()->PlaySceneChange = [this](SceneChangePoint & p)
			{
				if (this->isSceneChange) return;

				this->isSceneChange = true;
				this->playerEntryNumber = p.entryNumber;
				this->nextSceneName = p.nextSceneName;
			};
		}
	}

	Singleton<AudioClipManager>::Instance()->Play(AudioData::BGM_StageBGM01, 1.0f, true);
}

void GameSceneSystem::Update()
{
	if (this->isSceneChange)
	{
		//this->isSceneChange = false;

		Singleton<SceneManager>::Instance()->LoadSceneData(this->nextSceneName);
	}
}

void GameSceneSystem::LateUpdate()
{
}

void GameSceneSystem::OnDestroy()
{
	Singleton<AudioClipManager>::Instance()->Stop(AudioData::BGM_StageBGM01);
}
