#include "TextureManager.h"
#include "DirectX/ImGui/imgui.h"
#include "../../FrameWork/Input/Mouse.h"

#include <cereal/archives/json.hpp>
#include <fstream>

using namespace MyDirectX;
using namespace Input;

static const char * ManagerName = "TextureManager.json";
static const char * TEXTURE = "TEXTURE";

TextureManager::TextureManager()
{

}

TextureManager::~TextureManager()
{
	textureMap.clear();
}

void TextureManager::SaveTexture(std::string path)
{
	std::vector<std::string> texFileArray;

	// メッシュデータを一つずつ保存
	for (auto & tex : textureMap)
	{
		texFileArray.emplace_back(tex.first);
	}

	// 保存したデータの名前配列を出力
	std::string fileName = path + ManagerName;
	std::ofstream ofs(fileName, std::ios::out);
	{
		cereal::JSONOutputArchive o_archive(ofs);
		o_archive(cereal::make_nvp("FileNameArray", texFileArray));
	}
	ofs.close();
}

void TextureManager::LoadTexture(std::string path)
{
	std::vector<std::string> texFileArray;
	// メッシュデータの名前配列を読み込み
	std::string fileName = path + ManagerName;
	std::ifstream ifs(fileName, std::ios::in);
	if (!ifs) return;

	{
		cereal::JSONInputArchive i_archive(ifs);
		i_archive(texFileArray);
	}
	ifs.close();

	if (texFileArray.size() == 0)
		return;

	for (auto & tex : texFileArray)
	{
		Load(tex);
	}
}

std::weak_ptr<Texture> TextureManager::Load(std::string fileName)
{
	if (fileName.empty()) return std::weak_ptr<Texture>();

	std::weak_ptr<Texture> hitTex;
	//同じファイルが見つかったらロードしない
	for (auto & tex : textureMap)
	{
		size_t pos = tex.first.find_last_of("\\/");
		std::string texName1 = tex.first.substr(pos + 1, tex.first.size() - pos);
		pos = fileName.find_last_of("\\/");
		std::string texName2 = fileName.substr(pos + 1, fileName.size() - pos);

		if (texName1 == texName2)
		{
			hitTex = tex.second;
			break;
		}
	}
	// 同じファイルならその内容を返す
	if (!hitTex.expired())	
		return hitTex;

	std::shared_ptr<Texture> tex = std::shared_ptr<Texture>(new Texture());

	if (tex->Load(fileName))
	{
		// 成功したらマップに登録してそのポインタを返す
		tex->name = fileName;
		return textureMap[fileName] = tex;
	}
	
	return std::weak_ptr<Texture>();
}

std::weak_ptr<Texture> TextureManager::GetTexture(std::string fileName)
{
	if (textureMap.count(fileName) == 0)
		return std::weak_ptr<Texture>();
	
	return textureMap[fileName];
}

void TextureManager::DrawImGui()
{
	const Vector2 dragTexSize(35.0f, 35.0f);
	//const ImVec2 texSize(20.0f, 20.0f);

	int cnt = 0;
	for (auto & map : textureMap)
	{
		std::string id = std::to_string(cnt);

		BeginDragImGuiTexture(map.second, dragTexSize, id);
		cnt++;
	}
}

void TextureManager::BeginDragImGuiTexture(std::weak_ptr<Texture> texture, const Vector2 & size, std::string id)
{
	const ImVec2 texSize(size.x, size.y);
	const std::string texId = "##Texture" + id;

	// テクスチャをドラッグする
	auto DrapBegin = [&](std::string texName)
	{
		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
		{
			ImGui::SetDragDropPayload(TEXTURE, &texture, sizeof(texture));

			ImGui::Selectable((texName + texId).c_str());
			ImGui::Image((void*)*texture.lock()->GetShaderResourceView(), texSize);

			ImGui::EndDragDropSource();
		}
	};

	// テクスチャがないとき
	if (texture.expired())
	{
		ImGui::Button(("None" + texId).c_str(), texSize);
		ImGui::SameLine();
		ImGui::Selectable(("NoneTexure" + texId).c_str());
		return;
	}

	// パスは非表示
	size_t pos = texture.lock()->name.find_last_of("\\/");
	std::string texName = (pos == std::string::npos) ? texture.lock()->name : texture.lock()->name.substr(pos + 1, texture.lock()->name.size() - pos);

	ImGui::ImageButton((void*)*texture.lock()->GetShaderResourceView(), texSize);

	DrapBegin(texName);

	ImGui::SameLine();
	ImGui::Selectable((texName + texId).c_str());

	DrapBegin(texName);
}

bool TextureManager::DropTargetImGuiTexture(std::weak_ptr<Texture> & texture, const Vector2 & size, std::string id)
{
	static std::weak_ptr<Texture> * removeTexture = nullptr;
	const ImVec2 texSize(size.x, size.y);
	const std::string texId = "##Texture" + id;
	const std::string popUpName = "Texture" + texId;

	auto DrapTarget = [&texture]() -> bool
	{
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(TEXTURE))
			{
				auto & payload_n = *(std::weak_ptr<Texture>*)payload->Data;
				texture = payload_n;
				return !texture.expired();
			}
		}
		return false;
	};

	// テクスチャがないとき
	if (texture.expired())
	{
		ImGui::Button(("None" + texId).c_str(), texSize);

		if (DrapTarget()) return true;

		ImGui::SameLine();
		ImGui::Selectable(("NoneTexure" + texId).c_str());
	}
	else
	{
		ImGui::Image((void*)*texture.lock()->GetShaderResourceView(), texSize);

		if (DrapTarget()) return true;

		ImGui::SameLine();

		// パスは非表示
		size_t pos = texture.lock()->name.find_last_of("\\/");
		std::string texName = (pos == std::string::npos) ? texture.lock()->name : texture.lock()->name.substr(pos + 1, texture.lock()->name.size() - pos);
		ImGui::Selectable((texName + texId).c_str());

		if (ImGui::IsItemHovered())
		{
			if (Mouse::IsTrigger(Mouse::Button::Right))
			{
				removeTexture = &texture;
				ImGui::OpenPopup(popUpName.c_str());
			}
		}

		if (ImGui::BeginPopup(popUpName.c_str()))
		{
			if (removeTexture && !removeTexture->expired())
			{
				if (ImGui::MenuItem("Remove"))
				{
					removeTexture->reset();
					removeTexture = nullptr;
				}
			}

			ImGui::EndPopup();
		}
	}

	return DrapTarget();
}
