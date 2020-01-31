#include "SkinMeshManager.h"
#include "../../Assimp/AssimpLoader.h"
#include "../../../Cereal/Common.h"
#include "../../Renderer/SkinMeshRenderer.h"
#include "../../../FrameWork/Input/Mouse.h"

#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>
#include <fstream>
#include <direct.h>

using namespace MyDirectX;
using namespace Input;

static const char * ManagerName = "SkinMeshManager.json";
static const char * FileName = "SkinMesh";
static const char * SKINMESH = "SKINMESH";

void SkinMeshManager::SaveMesh(std::string path)
{
	// ���b�V���f�[�^�p�̃t�H���_�쐬
	std::string filePathName = path + std::string(FileName);
	_mkdir(filePathName.c_str());
	// ���b�V���f�[�^������ۑ�
	for (auto & name : loadSkinMeshNames)
	{
		std::shared_ptr<SkinMesh> mesh = skinMeshMap[name];
		if (mesh)
		{
			std::string meshName = name + ".skinmesh";

			std::string fileName = path + FileName + "/" + meshName;
			std::ofstream ofs(fileName, std::ios::binary);	// �o�C�i���Ȃ�std::ios::binary�Ƃ�������Ə���!!
			{
				cereal::BinaryOutputArchive o_archive(ofs);
				o_archive(mesh);
			}
			ofs.close();
		}
	}
}

void SkinMeshManager::LoadMesh(std::string path)
{
	std::vector<std::string> meshFileArray;
	// ���b�V���f�[�^�̖��O�z���ǂݍ���
	meshFileArray = FileLoader::GetFileListInFolder(path + FileName, "skinmesh");

	if (meshFileArray.size() == 0)
		return;

	// �X�V����܂őޔ�
	auto dethMap = skinMeshMap;
	skinMeshMap.clear();

	// ���O�����Ƀ��b�V����ǂݍ���
	for (auto & meshName : meshFileArray)
	{
		std::string fileName = path + FileName + "/" + meshName;
		std::ifstream ifs(fileName, std::ios::binary);	// �o�C�i���Ȃ�std::ios::binary�Ƃ�������Ə���!!

		if (ifs)
		{
			std::shared_ptr<SkinMesh> data;

			cereal::BinaryInputArchive i_archive(ifs);
			i_archive(data);

			data->Initialize();
			skinMeshMap[data->name] = data;
		}
		ifs.close();
	}

	//auto UpdateMeshRenderer = [&](std::list<std::weak_ptr<SkinMeshRenderer>> list)
	//{
	//	for (auto & renderer : list)
	//	{
	//		renderer.lock()->model = GetMesh(renderer.lock()->model.lock()->name);
	//		renderer.lock()->LoadModel();
	//	}
	//};

	//// ���ݑ��݂��Ă��郌���_���[�̍X�V������
	//UpdateMeshRenderer(Component::FindComponents<SkinMeshRenderer>());
	//// �v���n�u���X�V
	//auto scene = Singleton<SceneManager>::Instance();
	//if (scene)
	//{
	//	UpdateMeshRenderer(scene->GetCurrentScene()->prefab.FindComponents<SkinMeshRenderer>());
	//}

	// �X�V���I������̂ō폜
	dethMap.clear();
}

std::weak_ptr<SkinMesh> SkinMeshManager::Load(std::string fileName)
{
	SkinMesh * mesh = new SkinMesh();

	if (Assimp::AssimpLoader::LoadSkinMesh(fileName, mesh))
	{
		std::shared_ptr<SkinMesh> newMesh = std::shared_ptr<SkinMesh>(mesh);
		// �����̃A�j���[�V�����f�[�^���폜
		Singleton<AnimationClipManager>::Instance()->DeleteAnimation(newMesh->skeleton.get());
		newMesh->Initialize();

		auto oldMesh = skinMeshMap[newMesh->name];

		auto UpdateMeshRenderer = [&](std::list<std::weak_ptr<SkinMeshRenderer>> list)
		{
			for (auto & renderer : list)
			{
				if (renderer.lock()->model.lock() == oldMesh)
				{
					renderer.lock()->model = newMesh;
					renderer.lock()->LoadModel();
				}
			}
		};

		if (oldMesh)
		{
			// ���ݑ��݂��Ă��郌���_���[�̍X�V������
			UpdateMeshRenderer(Component::FindComponents<SkinMeshRenderer>());
			// �v���n�u���X�V
			auto scene = Singleton<SceneManager>::Instance();
			if (scene)
				UpdateMeshRenderer(scene->GetCurrentScene()->prefab.FindComponents<SkinMeshRenderer>());
		}

		loadSkinMeshNames.emplace_back(newMesh->name);
		skinMeshMap[newMesh->name] = newMesh;
		return skinMeshMap[newMesh->name];
	}
	else
	{
		delete mesh;
	}

	return std::weak_ptr<SkinMesh>();
}

std::weak_ptr<SkinMesh> MyDirectX::SkinMeshManager::GetMesh(std::string fileName)
{
	if (skinMeshMap.count(fileName) == 0)
		return std::weak_ptr<SkinMesh>();

	return skinMeshMap[fileName];
}

void SkinMeshManager::DrawImGui()
{
	int cnt = 0;
	for (auto & map : skinMeshMap)
	{
		std::string id = std::to_string(cnt);

		BeginDragImGuiMesh(map.second, id);
		cnt++;
	}
}

void SkinMeshManager::BeginDragImGuiMesh(std::weak_ptr<SkinMesh> mesh, std::string id)
{
	const std::string meshId = "##SkinMesh" + id;

	ImGui::Bullet(); ImGui::SameLine();
	ImGui::Selectable((mesh.lock()->name + meshId).c_str());

	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
	{
		ImGui::SetDragDropPayload(SKINMESH, &mesh, sizeof(mesh));

		ImGui::Text(mesh.lock()->name.c_str());

		ImGui::EndDragDropSource();
	}
}

bool SkinMeshManager::DropTargetImGuiMesh(std::weak_ptr<SkinMesh>& mesh, std::string id)
{
	static std::weak_ptr<SkinMesh> * removeMesh = nullptr;
	const std::string meshId = "##SkinMesh" + id;
	const std::string popUpName = "SkinMesh" + meshId;
	ImGui::AlignTextToFramePadding();

	ImGui::Text("SkinMesh"); ImGui::SameLine();

	float width = ImGui::GetWindowWidth() - ImGui::GetCursorPosX();

	// �f�[�^���Ȃ��Ƃ�
	if (mesh.expired())
	{
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.2f, 0.2f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.4f, 0.4f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
		ImGui::Button(("None" + meshId).c_str(), ImVec2(width, ImGui::GetFrameHeight()));
		ImGui::PopStyleColor(4);
	}
	else
	{
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.4f, 1.0f, 0.4f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.6f, 1.0f, 0.6f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 1.0f, 0.2f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
		ImGui::Button((mesh.lock()->name + meshId).c_str(), ImVec2(width, ImGui::GetFrameHeight()));
		ImGui::PopStyleColor(4);

		if (ImGui::IsItemHovered())
		{
			if (Mouse::IsTrigger(Mouse::Button::Right))
			{
				removeMesh = &mesh;
				ImGui::OpenPopup(popUpName.c_str());
			}
		}
	}

	if (ImGui::BeginPopup(popUpName.c_str()))
	{
		if (removeMesh && !removeMesh->expired())
		{
			if (ImGui::MenuItem("Remove"))
			{
				removeMesh->reset();
				removeMesh = nullptr;
			}
		}

		ImGui::EndPopup();
	}

	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(SKINMESH))
		{
			auto & payload_n = *(std::weak_ptr<SkinMesh>*)payload->Data;
			if (mesh.expired() || (!mesh.expired() && mesh.lock() != payload_n.lock()))
			{
				mesh = payload_n;
				return true;
			}
		}
	}
	return false;
}
