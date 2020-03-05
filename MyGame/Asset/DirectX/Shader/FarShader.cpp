#include "FarShader.h"
#include "../Material/Material.h"

using namespace MyDirectX;

void FarShader::Draw(Transform * transform, const Material * material, const MeshData<VTX_MESH>* mesh, unsigned short startIndex, unsigned short indexNum)
{
	// 通常描画

	VERTEX_SHADER_TYPE vertex = VERTEX_SHADER_TYPE::MESH;
	PIXEL_SHADER_TYPE pixel;
	// テクスチャなし
	if (material->pTexture.expired())
		pixel = PIXEL_SHADER_TYPE::MESH_NONE_TEXTURE;
	else
	{
		// テクスチャあり
		if (material->pNormalTexture.expired())
			pixel = PIXEL_SHADER_TYPE::MESH_TEXTURE;
		// 法線テクスチャあり
		else
		{
			vertex = VERTEX_SHADER_TYPE::MESH_NORMAL_TEXTURE;
			pixel = PIXEL_SHADER_TYPE::MESH_NORMAL_TEXTURE;
		}
	}

	SetVertexShaderFormat(vertex);
	SetPixelShaderFormat(pixel);
	ResetGeometryShaderFormat();

	ConstantBuffer::UpdateConstBuffer(CB_TYPE::CB_MATERIAL, material->materialParam);
	ConstantBuffer::SetPSRegister(1, CB_TYPE::CB_MATERIAL);
	material->SetPSResources();

	SetShader();

	mesh->DrawIndexed(indexNum, startIndex);


	// 毛の描画

	vertex = VERTEX_SHADER_TYPE::MESH_FAR_TEXTURE;
	pixel = PIXEL_SHADER_TYPE::MESH_FAR_TEXTURE;

	// 法線テクスチャあり
	if (!material->pNormalTexture.expired())
	{
		vertex = VERTEX_SHADER_TYPE::MESH_FAR_NORMAL_TEXTURE;
		pixel = PIXEL_SHADER_TYPE::MESH_FAR_NORMAL_TEXTURE;
	}

	SetVertexShaderFormat(vertex);
	SetPixelShaderFormat(pixel);
	ResetGeometryShaderFormat();

	// ファーテクスチャの設定
	RendererSystem::SetTexture(4, farTexture);

	ConstantBuffer::UpdateConstBuffer(CB_TYPE::CB_MATERIAL, material->materialParam);
	ConstantBuffer::SetPSRegister(1, CB_TYPE::CB_MATERIAL);
	material->SetPSResources();
	// パラメータの受け渡し
	CB_FAR cbFar;
	cbFar.world = transform->GetWorldMatrix();
	cbFar.maxNum = instanceCount;
	cbFar.scaleOffset = scaleOffset;
	ConstantBuffer::UpdateConstBuffer(CB_TYPE::CB_FAR, cbFar);
	ConstantBuffer::SetVSRegister(0, CB_TYPE::CB_FAR);

	SetShader();

	mesh->DrawIndexedInstanced(instanceCount, indexNum, startIndex);

	// パラメータの受け渡し
	ConstantBuffer::UpdateConstBuffer(CB_TYPE::CB_WORLD, transform->GetWorldMatrix());
	ConstantBuffer::SetVSRegister(0, CB_TYPE::CB_WORLD);
}

void FarShader::Draw(Transform * transform, const Material * material, const MeshData<VTX_SKIN_MESH>* mesh, unsigned short startIndex, unsigned short indexNum)
{
	// 通常描画

	VERTEX_SHADER_TYPE vertex = VERTEX_SHADER_TYPE::SKINMESH;
	PIXEL_SHADER_TYPE pixel;
	// テクスチャなし
	if (material->pTexture.expired())
		pixel = PIXEL_SHADER_TYPE::MESH_NONE_TEXTURE;
	else
	{
		// テクスチャあり
		if (material->pNormalTexture.expired())
			pixel = PIXEL_SHADER_TYPE::MESH_TEXTURE;
		// 法線テクスチャあり
		else
		{
			vertex = VERTEX_SHADER_TYPE::SKINMESH_NORMAL_TEXTURE;
			pixel = PIXEL_SHADER_TYPE::MESH_NORMAL_TEXTURE;
		}
	}

	SetVertexShaderFormat(vertex);
	SetPixelShaderFormat(pixel);
	ResetGeometryShaderFormat();

	ConstantBuffer::UpdateConstBuffer(CB_TYPE::CB_MATERIAL, material->materialParam);
	ConstantBuffer::SetPSRegister(1, CB_TYPE::CB_MATERIAL);
	material->SetPSResources();

	SetShader();

	mesh->DrawIndexed(indexNum, startIndex);


	// 毛の描画

	vertex = VERTEX_SHADER_TYPE::SKINMESH_FAR_TEXTURE;
	pixel = PIXEL_SHADER_TYPE::MESH_FAR_TEXTURE;
	
	// 法線テクスチャあり
	if (!material->pNormalTexture.expired())
	{
		vertex = VERTEX_SHADER_TYPE::SKINMESH_FAR_NORMAL_TEXTURE;
		pixel = PIXEL_SHADER_TYPE::MESH_FAR_NORMAL_TEXTURE;
	}

	SetVertexShaderFormat(vertex);
	SetPixelShaderFormat(pixel);
	ResetGeometryShaderFormat();

	// ファーテクスチャの設定
	RendererSystem::SetTexture(4, farTexture);

	ConstantBuffer::UpdateConstBuffer(CB_TYPE::CB_MATERIAL, material->materialParam);
	ConstantBuffer::SetPSRegister(1, CB_TYPE::CB_MATERIAL);
	material->SetPSResources();
	// パラメータの受け渡し
	CB_FAR cbFar;
	cbFar.world = transform->GetWorldMatrix();
	cbFar.maxNum = instanceCount;
	cbFar.scaleOffset = scaleOffset;
	ConstantBuffer::UpdateConstBuffer(CB_TYPE::CB_FAR, cbFar);
	ConstantBuffer::SetVSRegister(0, CB_TYPE::CB_FAR);

	SetShader();

	mesh->DrawIndexedInstanced(instanceCount, indexNum, startIndex);

	// パラメータの受け渡し
	ConstantBuffer::UpdateConstBuffer(CB_TYPE::CB_WORLD, transform->GetWorldMatrix());
	ConstantBuffer::SetVSRegister(0, CB_TYPE::CB_WORLD);
}

void FarShader::DrawImGui(Material * material, int & id)
{
	std::string texId;
	Vector2 texSize(35.0f, 35.0f);

	ImGui::Text("FarTexture");
	texId = "##FarTexture" + std::to_string(id); ++id;
	Singleton<TextureManager>::Instance()->DropTargetImGuiTexture(farTexture, texSize, texId);

	ImGui::DragScalar("Instance Count", ImGuiDataType_U32, &instanceCount, 1.0f);
	ImGui::DragFloat("Scale Offset", &scaleOffset, 0.001f, 0.0f, 100.0f);

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
}
