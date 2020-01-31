#include "MeshManager.h"
#include "../../Assimp/AssimpLoader.h"
#include "../../../Cereal/Common.h"
#include "../../Renderer/MeshRenderer.h"
#include "../../../FrameWork/Input/Mouse.h"

#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>
#include <fstream>
#include <iostream>
#include <sstream>
#include <direct.h>

using namespace MyDirectX;
using namespace Input;

static const char * ManagerName = "MeshManager.json";
static const char * FileName = "Mesh";
static const char * MESH = "MESH";
 
void MeshManager::SaveMesh(std::string path)
{
	// メッシュデータ用のフォルダ作成
	std::string filePathName = path + std::string(FileName);
	_mkdir(filePathName.c_str());
	// メッシュデータを一つずつ保存
	for (auto & name : loadMeshNames)
	{
		std::shared_ptr<Mesh> mesh = meshMap[name];
		if (mesh)
		{
			std::string meshName = name + ".mesh";

			std::string fileName = path + FileName + "/" + meshName;
			std::ofstream ofs(fileName, std::ios::binary);	// バイナリならstd::ios::binaryとしっかりと書く!!
			{
				cereal::BinaryOutputArchive o_archive(ofs);
				o_archive(mesh);
			}
			ofs.close();
		}
	}
}

void MeshManager::LoadMesh(std::string path)
{
	std::vector<std::string> meshFileArray;

	// メッシュデータの名前配列を読み込み
	meshFileArray = FileLoader::GetFileListInFolder(path + FileName, "mesh");

	if (meshFileArray.size() == 0)
		return;

	// 更新するまで退避
	auto dethMap = meshMap;
	meshMap.clear();

	// 名前を元にメッシュを読み込む
	for (auto & meshName : meshFileArray)
	{
		std::string fileName = path + FileName + "/" + meshName;
		std::ifstream ifs(fileName, std::ios::binary);	// バイナリならstd::ios::binaryとしっかりと書く!!
		
		if(ifs)
		{
			std::shared_ptr<Mesh> data;

			cereal::BinaryInputArchive i_archive(ifs);
			i_archive(data);

			data->Initialize();
			meshMap[data->name] = data;
		}
		ifs.close();
	}

	// 更新が終わったので削除
	dethMap.clear();
}

std::weak_ptr<Mesh> MeshManager::Load(std::string fileName)
{
	Mesh * mesh = new Mesh();

	if (Assimp::AssimpLoader::LoadMesh(fileName, mesh))
	{
		auto newMesh = std::shared_ptr<Mesh>(mesh);
		newMesh->Initialize();

		auto oldMesh = meshMap[newMesh->name];
		auto UpdateMeshRenderer = [&](std::list<std::weak_ptr<MeshRenderer>> list)
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
			// 現在存在しているレンダラーの更新をする
			UpdateMeshRenderer(Component::FindComponents<MeshRenderer>());
			// プレハブも更新
			auto scene = Singleton<SceneManager>::Instance();
			if (scene)
				UpdateMeshRenderer(scene->GetCurrentScene()->prefab.FindComponents<MeshRenderer>());
		}

		loadMeshNames.emplace_back(newMesh->name);
		meshMap[newMesh->name] = newMesh;
		return meshMap[newMesh->name];
	}
	else
	{
		delete mesh;
	}

	return std::shared_ptr<Mesh>();
}

std::weak_ptr<Mesh> MeshManager::GetMesh(std::string fileName)
{
	if (meshMap.count(fileName) == 0)
		return std::weak_ptr<Mesh>();

	return meshMap[fileName];
}

void MeshManager::DrawImGui()
{
	int cnt = 0;
	for (auto & map : meshMap)
	{
		std::string id = std::to_string(cnt);

		BeginDragImGuiMesh(map.second, id);
		cnt++;
	}
}

void MeshManager::BeginDragImGuiMesh(std::weak_ptr<Mesh> mesh, std::string id)
{
	const std::string meshId = "##Mesh" + id;

	ImGui::Bullet(); ImGui::SameLine();
	ImGui::Selectable((mesh.lock()->name + meshId).c_str());

	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
	{
		ImGui::SetDragDropPayload(MESH, &mesh, sizeof(mesh));

		ImGui::Text(mesh.lock()->name.c_str());

		ImGui::EndDragDropSource();
	}
}

bool MeshManager::DropTargetImGuiMesh(std::weak_ptr<Mesh>& mesh, std::string id)
{
	static std::weak_ptr<Mesh> * removeMesh = nullptr;
	const std::string meshId = "##Mesh" + id;
	const std::string popUpName = "Mesh" + meshId;
	ImGui::AlignTextToFramePadding();

	ImGui::Text("Mesh"); ImGui::SameLine();

	float width = ImGui::GetWindowWidth() - ImGui::GetCursorPosX();

	// データがないとき
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
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(MESH))
		{
			auto & payload_n = *(std::weak_ptr<Mesh>*)payload->Data;
			if (mesh.expired() || (!mesh.expired() && mesh.lock() != payload_n.lock()))
			{
				mesh = payload_n;
				return true;
			}
		}
	}
	return false;
}
