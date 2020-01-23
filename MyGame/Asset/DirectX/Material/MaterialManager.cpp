#include "MaterialManager.h"

#include "../Common.h"

#include <cereal/archives/json.hpp>
#include <cereal/archives/binary.hpp>
#include <fstream>
#include <direct.h>

using namespace MyDirectX;
using namespace Input;

static const char * FileName = "Material";
static const char * MATERIAL = "MATERIAL";

static const char * MaterialIdit = "MaterialIdit";
static const char * PopupMenuMaterial = "PopupMenuMaterial";
static const char * PopupMenuShader = "PopupMenuShader";

static BindClass<Shader> shaderList;

void MaterialManager::CreateShaderList(BindClass<Shader> sList)
{
	shaderList = sList;
}

void MaterialManager::SaveMaterial(std::string path)
{
	// データ用のフォルダ作成
	std::string filePathName = path + std::string(FileName);
	_mkdir(filePathName.c_str());

	// 削除されたデータのファイルを消す
	for (std::string delPath : deletePathList)
	{
		std::string name = delPath + ".material";
		DeleteFile((filePathName + "/" + name).c_str());
	}
	deletePathList.clear();

	// データを一つずつ保存
	for (auto & mat : materialList)
	{
		std::string name = mat->name + ".material";

		std::string fileName = path + FileName + "/" + name;
		std::ofstream ofs(fileName, std::ios::out);
		{
			cereal::JSONOutputArchive o_archive(ofs);
			o_archive(cereal::make_nvp(mat->name, mat));
		}
		ofs.close();
	}
}

void MaterialManager::LoadMaterial(std::string path)
{
	deletePathList.clear();

	std::vector<std::string> fileArray;
	// データの名前配列を読み込み
	fileArray = FileLoader::GetFileListInFolder(path + FileName, "material");

	if (fileArray.size() == 0)
		return;

	// 一旦退避
	auto dummyList = materialList;
	materialList.clear();

	std::string filePathName = path + std::string(FileName);
	// 名前配列からデータを読み込み
	for (auto & mat : fileArray)
	{
		std::ifstream ifs(filePathName + "/" + mat, std::ios::in);
		if (ifs)
		{
			std::shared_ptr<Material> inMat;
			cereal::JSONInputArchive i_archive(ifs);
			i_archive(inMat);
			materialList.emplace_back(inMat);
		}
		ifs.close();
	}


	auto UpdateMeshRenderer = [&](std::list<std::weak_ptr<MeshRenderer>> list)
	{
		for (auto & renderer : list)
		{
			for (auto & mat : renderer.lock()->materialArray)
			{
				if (mat.expired()) continue;

				mat = GetMaterial(mat.lock()->name);
			}
		}
	};
	auto UpdateSkinMeshRenderer = [&](std::list<std::weak_ptr<SkinMeshRenderer>> list)
	{
		for (auto & renderer : list)
		{
			for (auto & mat : renderer.lock()->materialArray)
			{
				if (mat.expired()) continue;

				mat = GetMaterial(mat.lock()->name);
			}
		}
	};

	// 現在存在しているレンダラーの更新をする
	UpdateMeshRenderer(Component::FindComponents<MeshRenderer>());
	UpdateSkinMeshRenderer(Component::FindComponents<SkinMeshRenderer>());

	// プレハブの更新
	auto scene = Singleton<SceneManager>::Instance();
	if (scene)
	{
		UpdateMeshRenderer(scene->GetCurrentScene()->prefab.FindComponents<MeshRenderer>());
		UpdateSkinMeshRenderer(scene->GetCurrentScene()->prefab.FindComponents<SkinMeshRenderer>());
	}

	// 更新が終わったので削除
	dummyList.clear();
}

void MaterialManager::DrawImGui()
{
	if (ImGui::IsWindowHovered())
	{
		if (Mouse::IsTrigger(Mouse::Button::Left))
			selectMaterial.reset();

		if (Mouse::IsTrigger(Mouse::Button::Right))
		{
			selectMaterial.reset();
			ImGui::OpenPopup(PopupMenuMaterial);
		}
	}

	int cnt = 0;
	for (auto & mat : materialList)
	{
		std::string id = std::to_string(cnt);

		BeginDragImGuiMaterial(mat, id);
		cnt++;
	}

	// マテリアル編集ウィンドウ
	bool isOpen = !selectMaterial.expired();
	if (isOpen)
	{
		std::weak_ptr<Material>& material = selectMaterial;
		std::string matId = "##Material" + std::to_string(cnt);
		if (ImGui::Begin(MaterialIdit, &isOpen))
		{
			ImGui::Text("Name"); ImGui::SameLine();

			std::string & matName = material.lock()->name;
			char inName[32] = {};
			if (!material.lock()->name.empty())
				memcpy(inName, &matName.front(), matName.size());
			if (ImGui::InputText((matId + "InputName").c_str(), inName, ARRAYSIZE(inName)))
			{
				matName = inName;
				std::shared_ptr<Material> setMat = material.lock();
				SetOriginalMaterialName(setMat);
			}

			std::string shaderName = "None";
			if(material.lock()->shader)
				shaderName = shaderList.GetBindClassName(*material.lock()->shader.get());

			ImGui::Text("Shader"); ImGui::SameLine();

			float width = ImGui::GetWindowWidth() - ImGui::GetCursorPosX();
			if (ImGui::Button((shaderName + matId).c_str(), ImVec2(width, ImGui::GetFrameHeight())))
			{
				ImGui::OpenPopup(PopupMenuShader);
			}

			material.lock()->DrawImGui(cnt);

			if (ImGui::BeginPopup(PopupMenuShader))
			{
				ImGui::BeginChild("Child##Shader", ImVec2(250, 200), true);
				for (auto & shaderData : shaderList.list)
				{
					if (ImGui::MenuItem((shaderData->name + "##Shader").c_str()))
					{
						ImGui::CloseCurrentPopup();
						material.lock()->SetShader(shaderData->instantiate());	// シェーダーのインスタンス生成
					}
				}
				ImGui::EndChild();

				ImGui::EndPopup();
			}
		}
		ImGui::End();

		if (!isOpen)
			selectMaterial.reset();
	}

	if (ImGui::BeginPopup(PopupMenuMaterial))
	{
		if (ImGui::MenuItem("Create Material"))
		{
			std::shared_ptr<Material> newMat = std::make_shared<Material>();
			newMat->name = "New Material";
			SetOriginalMaterialName(newMat);
			materialList.emplace_back(newMat);
		}
		if (!selectMaterial.expired())
		{
			if (ImGui::MenuItem("Delete"))
			{
				for (auto itr = materialList.begin(), end = materialList.end(); itr != end; ++itr)
				{
					if (selectMaterial.lock() == *itr)
					{
						deletePathList.emplace_back(selectMaterial.lock()->name);
						selectMaterial.reset();
						materialList.erase(itr);
						break;
					}
				}
			}
		}

		ImGui::EndPopup();
	}
}

std::weak_ptr<Material> MaterialManager::GetMaterial(std::string name)
{
	for (auto & mat : materialList)
	{
		if (mat->name == name)
		{
			return mat;
		}
	}

	return std::weak_ptr<Material>();
}

void MaterialManager::BeginDragImGuiMaterial(std::shared_ptr<Material> & material, std::string id)
{
	const std::string matId = "##Material" + id;

	std::string & matName = material->name;
	//ImGui::RadioButton((matId + "RadioButton").c_str(), true); ImGui::SameLine();
	ImGui::Bullet(); ImGui::SameLine();

	bool isSelect = material == selectMaterial.lock();
	if (ImGui::Selectable((matName + matId).c_str(), isSelect))
	{
		if (isSelect)
			selectMaterial.reset();
		else
			selectMaterial = material;
	}

	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
	{
		ImGui::SetDragDropPayload(MATERIAL, &material, sizeof(material));

		ImGui::Text(matName.c_str());

		ImGui::EndDragDropSource();
	}

	if (ImGui::IsItemHovered())
	{
		if (Mouse::IsTrigger(Mouse::Button::Right))
		{
			selectMaterial = material;
			ImGui::OpenPopup(PopupMenuMaterial);
		}
	}
}

bool MaterialManager::DropTargetImGuiMaterial(std::weak_ptr<Material> & material, std::string id)
{
	static std::weak_ptr<Material> * removeMaterial = nullptr;
	const std::string matId = "##Material" + id;
	const std::string popUpName = "Material" + matId;
	float width = ImGui::GetWindowWidth() - ImGui::GetCursorPosX();

	// データがないとき
	if (material.expired())
	{
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.2f, 0.2f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.4f, 0.4f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
		ImGui::Button(("None" + matId).c_str(), ImVec2(width, ImGui::GetFrameHeight()));
		ImGui::PopStyleColor(4);
	}
	else
	{
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.4f, 1.0f, 0.4f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.6f, 1.0f, 0.6f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 1.0f, 0.2f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
		bool isSelect = material.lock() == selectMaterial.lock();
		if (ImGui::Button((material.lock()->name + matId).c_str(), ImVec2(width, ImGui::GetFrameHeight())))
		{
			if (isSelect)
				selectMaterial.reset();
			else
				selectMaterial = material;
		}
		ImGui::PopStyleColor(4);

		if (ImGui::IsItemHovered())
		{
			if (Mouse::IsTrigger(Mouse::Button::Right))
			{
				removeMaterial = &material;
				ImGui::OpenPopup(popUpName.c_str());
			}
		}
	}

	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(MATERIAL))
		{
			auto & payload_n = *(std::shared_ptr<Material>*)payload->Data;
			material = payload_n;
			//// シリアル化してコピー
			//std::stringstream ss;
			//{
			//	cereal::BinaryOutputArchive o_archive(ss);
			//	o_archive(payload_n);
			//}
			//{
			//	cereal::BinaryInputArchive i_archive(ss);
			//	i_archive(material);
			//}
			return true;
		}
	}

	if (ImGui::BeginPopup(popUpName.c_str()))
	{
		if (removeMaterial && !removeMaterial->expired())
		{
			if (ImGui::MenuItem("Remove"))
			{
				removeMaterial->reset();
				removeMaterial = nullptr;
			}
		}

		ImGui::EndPopup();
	}

	return false;
}

void MaterialManager::SetOriginalMaterialName(std::shared_ptr<Material>& material)
{
	// 同じ名前がいなかったらそのままリターン
	if (CheckMaterialName(material, material->name))
		return;

	// 重複しなくなるまでカウントを増やす
	int cnt = 1;
	while (true)
	{
		if (CheckMaterialName(material, material->name + "(" + std::to_string(cnt) + ")"))
			break;
		cnt++;
	}
	material->name += "(" + std::to_string(cnt) + ")";
}

bool MaterialManager::CheckMaterialName(std::shared_ptr<Material> mine, std::string checkName)
{
	for (auto & mat : materialList)
	{
		if (mat == mine) continue;

		if (mat->name == checkName)
			return false;
	}
	return true;
}
