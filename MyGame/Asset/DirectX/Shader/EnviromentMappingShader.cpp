#include "EnviromentMappingShader.h"
#include "ConstantBuffer.h"
#include "../Material/Material.h"
#include "../Texture/TextureManager.h"

using namespace MyDirectX;

void EnviromentMappingShader::SetOption(const Material * material)
{
	VERTEX_SHADER_TYPE vertex = VERTEX_SHADER_TYPE::MESH;
	PIXEL_SHADER_TYPE pixel = PIXEL_SHADER_TYPE::MESH_ENVIROMENT_MAPPING_TEXTURE;

	SetVertexShaderFormat(vertex);
	SetPixelShaderFormat(pixel);
	ResetGeometryShaderFormat();

	ConstantBuffer::UpdateConstBuffer(CB_TYPE::CB_MATERIAL, material->materialParam);
	ConstantBuffer::SetPSRegister(1, CB_TYPE::CB_MATERIAL);
	material->SetPSResources();
}

void EnviromentMappingShader::DrawImGui(Material * material, int & id)
{
	std::string texId;
	Vector2 texSize(35.0f, 35.0f);
	ImGui::Text("EnviromentTexture");
	texId = "##TextureDrap" + std::to_string(id); ++id;
	Singleton<TextureManager>::Instance()->DropTargetImGuiTexture(material->pTexture, texSize, texId);

	std::string colId = texId;
	ImGui::ColorEdit4(("Diffuse" + colId).c_str(), material->materialParam.diffuse);
	ImGui::ColorEdit4(("Ambient" + colId).c_str(), material->materialParam.ambient);
	ImGui::ColorEdit3(("Specular" + colId).c_str(), material->materialParam.specular);
	ImGui::DragFloat(("Hardness" + colId).c_str(), &material->materialParam.specular.a, 0.01f, 0.0f, 50.0f);
	ImGui::DragFloat(("Reflection" + colId).c_str(), &material->materialParam.reflection, 0.01f, 0.0f, 50.0f);
	ImGui::ColorEdit3(("emissive" + colId).c_str(), material->materialParam.emissive);
	ImGui::DragFloat(("Shininess" + colId).c_str(), &material->materialParam.emissive.a, 0.01f, 0.0f, 100.0f);
}
