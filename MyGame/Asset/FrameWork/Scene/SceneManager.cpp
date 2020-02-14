#include "SceneManager.h"
#include "SceneBase.h"
#include "../GameObject/GameObjectManager.h"
#include "../../Main/manager.h"
#include <cereal/archives/binary.hpp>
#include <direct.h>
#include <fstream>
#include <iostream>

using namespace FrameWork;
using namespace Input;

static const char * FileName = "Scene";

SceneManager::~SceneManager()
{
	delete currentScene;
	if (nextScene != nullptr) delete nextScene;
}


void SceneManager::Initialize(SceneBase * scene)
{
	currentScene = scene;
	nextScene = nullptr;
	currentScene->Initialize();
	//currentScene->Start();

	LoadFileData(DATA_FOLDER_PATH);
}

void SceneManager::SceneChange(SceneBase * newScene)
{
	if (nextScene != nullptr) delete nextScene;

	nextScene = newScene;
}

SceneBase * SceneManager::GetCurrentScene()
{
	return currentScene;
}

void SceneManager::LoadSceneData(std::string sceneName)
{
	std::string filePathName = DATA_FOLDER_PATH + std::string(FileName);
	std::ifstream ifs(filePathName + "/" + sceneName + ".scene", std::ios::binary);
	if (!ifs) return;

	{
		std::shared_ptr<SceneData> newScene;
		cereal::BinaryInputArchive i_archive(ifs);
		i_archive(newScene);

		if (!newScene) return;

		newScene->name = sceneName;

		// ���݂̃I�u�W�F�N�g���폜
		DeleteSceneData(currentScene->sceneData);

		// �V�[���f�[�^����V�����I�u�W�F�N�g�𐶐�
		CreateSceneData(newScene);
	}

	// �Q�[���X�^�[�g
	currentScene->Start();
}

void SceneManager::LoadSceneData(std::stringstream & sceneData)
{
	std::shared_ptr<SceneData> newScene;
	cereal::BinaryInputArchive i_archive(sceneData);
	i_archive(newScene);

	if (!newScene) return;

	// ���݂̃I�u�W�F�N�g���폜
	DeleteSceneData(currentScene->sceneData);

	// �V�[���f�[�^����V�����I�u�W�F�N�g�𐶐�
	CreateSceneData(newScene);
}

void SceneManager::SaveSceneData()
{
	if (!currentScene)return;
	SaveSceneData(currentScene->sceneData);
}

void SceneManager::DrawImGui()
{
	std::string strId = "##SceneData";
	const char SCENEDATA_POPUP[] = "SCENEDATA_POPUP";
	const char SCENEDATA_EDIT[] = "SceneDataEdit";

	static std::weak_ptr<std::string> selectDataName;

	bool isOpen = !selectDataName.expired();
	if (ImGui::BeginChild(SCENEDATA_EDIT, ImVec2(0.0f, 55.0f)))
	{
		if (isOpen)
		{
			// �V�[���f�[�^�ҏW�E�B���h�E
			ImGui::Text("Name"); ImGui::SameLine();

			std::string beforeName = *selectDataName.lock();
			char inName[32] = {};
			if (!selectDataName.lock()->empty())
				memcpy(inName, &beforeName.front(), beforeName.size());
			if (ImGui::InputText((strId + "InputName").c_str(), inName, ARRAYSIZE(inName)))
			{
				*selectDataName.lock() = inName;
				SetOriginalName(selectDataName.lock().get());

				// ���݂̃V�[���̖��O���ύX����
				if (currentScene->sceneData && beforeName == currentScene->sceneData->name)
				{
					currentScene->sceneData->name = *selectDataName.lock();
				}

				// �t�@�C���ɂ��ύX���𔽉f
				std::string filePathName = DATA_FOLDER_PATH + std::string(FileName);
				std::string oldName = filePathName + +"/" + beforeName + ".scene";
				std::string finalName = filePathName + "/" + *selectDataName.lock() + ".scene";
				rename(oldName.c_str(), finalName.c_str());
			}

			float winWidth = ImGui::GetWindowWidth();
			if (ImGui::Button(("Load" + strId).c_str(), ImVec2(winWidth * 0.45f, 0.0f)))
			{
				ImGui::OpenPopup("Load?");
			}
			ImGui::SameLine();
			if (ImGui::Button(("Delete" + strId).c_str(), ImVec2(winWidth * 0.45f, 0)))
			{
				ImGui::OpenPopup("Delete?");

			}

			if (ImGui::BeginPopupModal("Load?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
			{
				ImGui::Text("final confirmation.");
				if (ImGui::Button("OK", ImVec2(120, 0)))
				{
					LoadSceneData(*selectDataName.lock());
					selectDataName.reset();
					ImGui::CloseCurrentPopup();
				}
				ImGui::SetItemDefaultFocus();
				ImGui::SameLine();
				if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
				ImGui::EndPopup();
			}
			if (ImGui::BeginPopupModal("Delete?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
			{
				ImGui::Text("final confirmation.");
				if (ImGui::Button("OK", ImVec2(120, 0)))
				{
					DeleteSceneFile(*selectDataName.lock());
					selectDataName.reset();
					ImGui::CloseCurrentPopup();
				}
				ImGui::SetItemDefaultFocus();
				ImGui::SameLine();
				if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
				ImGui::EndPopup();
			}

			if (!isOpen)
				selectDataName.reset();
		}
	} ImGui::EndChild();

	ImGui::Separator();

	if (ImGui::IsWindowHovered())
	{
		if (Mouse::IsTrigger(Mouse::Button::Left))
		{
			selectDataName.reset();
		}
	}

	for (auto & scene : sceneNameArray)
	{
		size_t pos = scene->find_last_of(".");	// �g���q�͊O��
		std::string sceneName = scene->substr(0, pos);

		bool isSelect = (!selectDataName.expired()) ? selectDataName.lock() == scene : false;
		if (ImGui::Selectable((sceneName + strId).c_str(), isSelect))
		{
			if (isSelect)
				selectDataName.expired();
			else
				selectDataName = scene;
		}

		if (ImGui::IsItemHovered())
		{
			if (Mouse::IsTrigger(Mouse::Button::Right))
			{
				selectDataName = scene;
				ImGui::OpenPopup(SCENEDATA_POPUP);
			}
		}
	}

	if (ImGui::IsWindowHovered() && !ImGui::IsPopupOpen(SCENEDATA_POPUP))
	{
		if (Mouse::IsTrigger(Mouse::Button::Right))
		{
			ImGui::OpenPopup(SCENEDATA_POPUP);
		}
	}

	if (ImGui::BeginPopup(SCENEDATA_POPUP))
	{
		if (ImGui::MenuItem("Create Scene"))
		{
			std::shared_ptr<SceneData> newData = std::shared_ptr<SceneData>(new SceneData());
			newData->name = "New Scene";
			SetOriginalName(&newData->name);

			// �V�����V�[���̖��O��z��ɒǉ�
			sceneNameArray.emplace_back(new std::string(newData->name));

			SaveSceneData(newData);
		}
		ImGui::EndPopup();
	}
}

void SceneManager::Update()
{
	currentScene->Update();
}

void SceneManager::Draw()
{
	currentScene->Draw();
}

void SceneManager::SceneCheck()
{
	if (nextScene == nullptr) return;

	if (currentScene->GetType() != nextScene->GetType())
	{
		delete currentScene;
		currentScene = nextScene;

		currentScene->Initialize();
		//currentScene->Start();
	}

	nextScene = nullptr;
}

void SceneManager::RemoveSceneGameObject(std::weak_ptr<GameObject> remove)
{
	if (!currentScene->sceneData) return;

	auto & objectList = currentScene->sceneData->gameObjectList;
	for (auto itr = objectList.begin(), end = objectList.end(); itr != end; ++itr)
	{
		if (itr->lock() == remove.lock())
		{
			objectList.erase(itr);
			return;
		}
	}
}

void SceneManager::LoadFileData(std::string path)
{
	// �f�[�^�̖��O�z���ǂݍ���
	auto NameArray = FileLoader::GetFileListInFolder(DATA_FOLDER_PATH + FileName, "scene");

	for (std::string & file : NameArray)
	{
		size_t pos = file.find_last_of(".");	// �g���q�͊O��
		std::string name = file.substr(0, pos);
		sceneNameArray.emplace_back(new std::string(name));
	}
}

void SceneManager::SaveSceneData(std::shared_ptr<SceneData> sceneData)
{
	// �f�[�^�p�̃t�H���_�쐬
	std::string filePathName = DATA_FOLDER_PATH + std::string(FileName);
	_mkdir(filePathName.c_str());

	std::string name = sceneData->name + ".scene";

	std::string fileName = filePathName + "/" + name;
	std::ofstream ofs(fileName, std::ios::binary);
	{
		// �o�͗p�̃f�[�^�쐬
		std::shared_ptr<SceneData> saveData = std::shared_ptr<SceneData>(new SceneData());
		saveData->name = sceneData->name;
		// �e�����Ȃ��I�u�W�F�N�g������I��
		for (auto & obj : sceneData->gameObjectList)
		{
			if (obj.expired())continue;
			if (!obj.lock()->transform.lock()->GetParent().expired()) continue;	// �e�����������Ȃ�

			saveData->gameObjectList.emplace_back(obj);
		}

		cereal::BinaryOutputArchive o_archive(ofs);
		o_archive(saveData);
	}
	ofs.close();
}

void SceneManager::DeleteSceneFile(std::string delName)
{
	// ���݂̃V�[���Ȃ炻�̃f�[�^���폜
	if (currentScene->sceneData)
	{
		if (currentScene->sceneData->name == delName)
		{
			DeleteSceneData(currentScene->sceneData);
		}
	}
	// ���O�z�񂩂���폜
	for (auto itr = sceneNameArray.begin(),end = sceneNameArray.end();itr != end;++itr)
	{
		if (*(*itr) == delName)
		{
			sceneNameArray.erase(itr);
			break;
		}
	}

	// �f�[�^�p�̃t�H���_�쐬
	std::string filePathName = DATA_FOLDER_PATH + std::string(FileName);
	std::string name = delName + ".scene";
	std::string fileName = filePathName + "/" + name;
	// �t�@�C���̍폜
	DeleteFile(fileName.c_str());
}

void SceneManager::DeleteSceneData(std::shared_ptr<SceneData>& deleteData)
{
	if (currentScene->sceneData)
	{
		for (auto & object : currentScene->sceneData->gameObjectList)
		{
			GameObject::Destroy(object);
		}
		currentScene->sceneData.reset();

		// ���݂���폜�����I�u�W�F�N�g��S�Ĕj��
		Singleton<GameObjectManager>::Instance()->CleanupDestroyGameObject();
		Component::CleanupDestoryComponent();
	}
}

void SceneManager::CreateSceneData(std::shared_ptr<SceneData>& createData)
{
	// �����ŃV�[���f�[�^���㏑��
	currentScene->sceneData = createData;
	// �V�����V�[���̃I�u�W�F�N�g���}�l�[�W���[�ɓo�^
	Singleton<GameObjectManager>::Instance()->RegisterSceneDataGameObjects(currentScene->sceneData);
}

void SceneManager::SetOriginalName(std::string * original)
{
	// �������O�����Ȃ������炻�̂܂܃��^�[��
	if (CheckName(original, *original))
		return;

	// �d�����Ȃ��Ȃ�܂ŃJ�E���g�𑝂₷
	int cnt = 1;
	while (true)
	{
		if (CheckName(original, *original + "(" + std::to_string(cnt) + ")"))
			break;
		cnt++;
	}
	*original += "(" + std::to_string(cnt) + ")";
}

bool SceneManager::CheckName(std::string * mine, std::string checkName)
{
	for (auto & data : sceneNameArray)
	{
		if (data.get() == mine) continue;

		if (*data == checkName)
			return false;
	}
	return true;
}

