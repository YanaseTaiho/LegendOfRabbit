#include "Material.h"
#include "../Shader/Shader.h"
#include "../Texture/TextureManager.h"

using namespace MyDirectX;

void Material::LoadSerialize(std::string tex, std::string nor, std::string hei)
{
	pTexture = Singleton<TextureManager>::Instance()->Load(tex);
	pNormalTexture = Singleton<TextureManager>::Instance()->Load(nor);
	pHeightTexture = Singleton<TextureManager>::Instance()->Load(hei);
}

Material::Material()
{
	rasterizer = Rasterizer::FILL_SOLID_AND_CULL_BACK;
	type = (MaterialType)0;
}

Material::~Material()
{
	pTexture.reset();
	pNormalTexture.reset();
}

void Material::SetShader(Shader * shader)
{
	this->shader = std::shared_ptr<Shader>(shader);
}

void Material::DrawImGui(int & id)
{
	if (!this->shader) return;

	std::string strId = "##Material" + std::to_string(id);

	ImGui::Text("Rasterizer"); ImGui::SameLine();
	if (ImGui::Button((enum_to_string(rasterizer) + strId).c_str()))
	{
		ImGui::OpenPopup("Rasterizer##PopUp");
	}
	ImGui::Text("Type"); ImGui::SameLine();
	if (ImGui::Button((enum_to_string(type) + strId).c_str()))
	{
		ImGui::OpenPopup("MaterialType##PopUp");
	}

	if (ImGui::BeginPopup("Rasterizer##PopUp"))
	{
		for (int i = 0; i < (int)Rasterizer::MAX_NUM; i++)
		{
			if (ImGui::MenuItem(enum_to_string((Rasterizer)i).c_str()))
			{
				rasterizer = (Rasterizer)i;
			}
		}
		ImGui::EndPopup();
	}
	if (ImGui::BeginPopup("MaterialType##PopUp"))
	{
		for (int i = 0; i < (int)MaterialType::MaxNum; i++)
		{
			if (ImGui::MenuItem(enum_to_string((MaterialType)i).c_str()))
			{
				type = (MaterialType)i;
			}
		}
		ImGui::EndPopup();
	}

	shader->DrawImGui(this, id);
}

void Material::SetPSResources() const
{
	// テクスチャの設定
	RendererSystem::SetTexture(0, pTexture);
	// 法線テクスチャの設定
	RendererSystem::SetTexture(2, pNormalTexture);
	// ハイトマップテクスチャの設定
	RendererSystem::SetTexture(3, pHeightTexture);
}

void Material::SetTexture(std::string fileName)
{
	pTexture = Singleton<TextureManager>::Instance()->Load(fileName);
}

void Material::SetNormalTexture(std::string fileName)
{
	pNormalTexture = Singleton<TextureManager>::Instance()->Load(fileName);
}

void Material::SetHeightTexture(std::string fileName)
{
	pHeightTexture = Singleton<TextureManager>::Instance()->Load(fileName);
}
