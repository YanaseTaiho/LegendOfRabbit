#include "ToonShader.h"
#include "ConstantBuffer.h"
#include "../Material/Material.h"
#include "../Texture/TextureManager.h"

using namespace MyDirectX;

void ToonShader::Draw(Transform * transform, const Material * material, const MeshData<VTX_MESH>* mesh, unsigned short startIndex, unsigned short indexNum)
{
	VERTEX_SHADER_TYPE vertex = VERTEX_SHADER_TYPE::MESH;
	PIXEL_SHADER_TYPE pixel;
	// �e�N�X�`���Ȃ�
	if (material->pTexture.expired())
		pixel = PIXEL_SHADER_TYPE::MESH_NONE_TEXTURE;
	else
	{
		// �e�N�X�`������
		if (material->pNormalTexture.expired())
			pixel = PIXEL_SHADER_TYPE::MESH_TOON_TEXTURE;
		// �@���e�N�X�`������
		else
		{
			vertex = VERTEX_SHADER_TYPE::MESH_NORMAL_TEXTURE;
			pixel = PIXEL_SHADER_TYPE::MESH_TOON_TEXTURE;
		}
	}

	SetVertexShaderFormat(vertex);
	SetPixelShaderFormat(pixel);
	ResetGeometryShaderFormat();

	ConstantBuffer::UpdateConstBuffer(CB_TYPE::CB_MATERIAL, material->materialParam);
	ConstantBuffer::SetPSRegister(1, CB_TYPE::CB_MATERIAL);

	// �e�N�X�`���̐ݒ�
	RendererSystem::SetTexture(0, material->pTexture);
	// �g�D�[���e�N�X�`���̐ݒ�
	RendererSystem::SetTexture(2, toonTexture);
	// �@���e�N�X�`���̐ݒ�
	RendererSystem::SetTexture(3, material->pNormalTexture);
	// �n�C�g�}�b�v�e�N�X�`���̐ݒ�
	RendererSystem::SetTexture(4, material->pHeightTexture);

	SetShader();

	mesh->DrawIndexed(indexNum, startIndex);
}

void ToonShader::Draw(Transform * transform, const Material * material, const MeshData<VTX_SKIN_MESH>* mesh, unsigned short startIndex, unsigned short indexNum)
{
	VERTEX_SHADER_TYPE vertex = VERTEX_SHADER_TYPE::SKINMESH;
	PIXEL_SHADER_TYPE pixel;
	// �e�N�X�`���Ȃ�
	if (material->pTexture.expired())
		pixel = PIXEL_SHADER_TYPE::MESH_NONE_TEXTURE;
	else
	{
		// �e�N�X�`������
		if (material->pNormalTexture.expired())
			pixel = PIXEL_SHADER_TYPE::MESH_TOON_TEXTURE;
		// �@���e�N�X�`������
		else
		{
			vertex = VERTEX_SHADER_TYPE::SKINMESH_NORMAL_TEXTURE;
			pixel = PIXEL_SHADER_TYPE::MESH_TOON_TEXTURE;
		}
	}

	SetVertexShaderFormat(vertex);
	SetPixelShaderFormat(pixel);
	ResetGeometryShaderFormat();

	ConstantBuffer::UpdateConstBuffer(CB_TYPE::CB_MATERIAL, material->materialParam);
	ConstantBuffer::SetPSRegister(1, CB_TYPE::CB_MATERIAL);

	// �e�N�X�`���̐ݒ�
	RendererSystem::SetTexture(0, material->pTexture);
	// �g�D�[���e�N�X�`���̐ݒ�
	RendererSystem::SetTexture(2, toonTexture);
	// �@���e�N�X�`���̐ݒ�
	RendererSystem::SetTexture(3, material->pNormalTexture);
	// �n�C�g�}�b�v�e�N�X�`���̐ݒ�
	RendererSystem::SetTexture(4, material->pHeightTexture);

	SetShader();

	mesh->DrawIndexed(indexNum, startIndex);
}

void ToonShader::DrawImGui(Material * material, int & id)
{
	std::string texId;
	Vector2 texSize(35.0f, 35.0f);
	ImGui::Text("ToonTexture");
	texId = "##TextureDrap" + std::to_string(id); ++id;
	Singleton<TextureManager>::Instance()->DropTargetImGuiTexture(toonTexture, texSize, texId);
	ImGui::Text("DiffuseTexture");
	texId = "##TextureDrap" + std::to_string(id); ++id;
	Singleton<TextureManager>::Instance()->DropTargetImGuiTexture(material->pTexture, texSize, texId);
	ImGui::Text("NormalTexture");
	texId = "##NormalTexture" + std::to_string(id); ++id;
	Singleton<TextureManager>::Instance()->DropTargetImGuiTexture(material->pNormalTexture, texSize, texId);
	ImGui::Text("HeightTexture");
	texId = "##HeightTexture" + std::to_string(id); ++id;
	Singleton<TextureManager>::Instance()->DropTargetImGuiTexture(material->pHeightTexture, texSize, texId);

	std::string colId = texId;
	ImGui::ColorEdit4(("Diffuse" + colId).c_str(), material->materialParam.diffuse);
	ImGui::ColorEdit4(("Ambient" + colId).c_str(), material->materialParam.ambient);
	ImGui::ColorEdit3(("Specular" + colId).c_str(), material->materialParam.specular);
	ImGui::DragFloat(("Hardness" + colId).c_str(), &material->materialParam.specular.a, 0.01f, 0.0f, 50.0f);
	ImGui::DragFloat(("Reflection" + colId).c_str(), &material->materialParam.reflection, 0.01f, 0.0f, 50.0f);
	ImGui::ColorEdit3(("emissive" + colId).c_str(), material->materialParam.emissive);
	ImGui::DragFloat(("Shininess" + colId).c_str(), &material->materialParam.emissive.a, 0.01f, 0.0f, 100.0f);

	if (!material->pNormalTexture.expired())
	{
		ImGui::ColorEdit3(("BumpDiffuse" + colId).c_str(), material->materialParam.bump);
	}
	if (!material->pHeightTexture.expired())
	{
		ImGui::DragFloat(("HeightFactor" + colId).c_str(), &material->materialParam.height, 0.001f, 0.0f, 1.0f);
	}
	else
		material->materialParam.height = 0.0f;
}
