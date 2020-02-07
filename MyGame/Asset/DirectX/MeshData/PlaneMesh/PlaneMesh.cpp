#include "PlaneMesh.h"
#include "../../Shader/ConstantBuffer.h"
#include "../../Shader/Shader.h"

using namespace MyDirectX;

MeshData<VTX_PLANE_MESH> PlaneMesh::meshData;
std::shared_ptr<Material> PlaneMesh::material;

void PlaneMesh::CreateMesh()
{
	std::vector<VTX_PLANE_MESH> vertex(4);

	vertex[0].pos = Vector3(-0.5f, -0.5f, 0.0f);
	vertex[0].nor = Vector3(0.0f, 1.0f, 0.0f);
	vertex[0].uv = Vector2(0.0f, 0.0f);

	vertex[1].pos = Vector3(0.5f, -0.5f, 0.0f);
	vertex[1].nor = Vector3(0.0f, 1.0f, 0.0f);
	vertex[1].uv = Vector2(1.0f, 0.0f);

	vertex[2].pos = Vector3(-0.5f, 0.5f, 0.0f);
	vertex[2].nor = Vector3(0.0f, 1.0f, 0.0f);
	vertex[2].uv = Vector2(0.0f, 1.0f);

	vertex[3].pos = Vector3(0.5f, 0.5f, 0.0f);
	vertex[3].nor = Vector3(0.0f, 1.0f, 0.0f);
	vertex[3].uv = Vector2(1.0f, 1.0f);

	meshData.CreateVertexBuffer(&vertex);

	material = std::make_shared<Material>();
	material->materialParam.diffuse = Color::white();
	material->materialParam.ambient = Color::black();
	material->materialParam.specular = Color::black();
	material->materialParam.emissive = Color::black();
	material->materialParam.bump = Color::black();
	material->materialParam.shininess = 0.0f;
	material->materialParam.specularFactor = 0.0f;
	material->materialParam.reflection = 0.0f;
}

void PlaneMesh::ReleaseMesh()
{
	material.reset();
}

std::weak_ptr<Material> MyDirectX::PlaneMesh::GetMaterial()
{
	return std::weak_ptr<Material>();
}

void PlaneMesh::SetTexture(std::weak_ptr<Texture> texture)
{
	material->pTexture = texture;
}

void PlaneMesh::SetUV(float u1, float v1, float u2, float v2)
{
	meshData.vertex[0].uv = Vector2(u1,v1);
	meshData.vertex[1].uv = Vector2(u2,v1);
	meshData.vertex[2].uv = Vector2(u1,v2);
	meshData.vertex[3].uv = Vector2(u2,v2);
}

void PlaneMesh::Draw(float left, float right, float top, float bottom)
{
	RendererSystem::GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	meshData.vertex[0].pos = Vector3(left, top, 0.0f);
	meshData.vertex[1].pos = Vector3(right, top, 0.0f);
	meshData.vertex[2].pos = Vector3(left, bottom, 0.0f);
	meshData.vertex[3].pos = Vector3(right, bottom, 0.0f);
	// 頂点データ変更時この関数でバッファー更新
	meshData.SetVertexBuffer();

	auto manager = ShaderFormatManager::Instance();
	Shader::SetShader(manager->GetVertexFormat(VERTEX_SHADER_TYPE::MESH), manager->GetPixelFormat(PIXEL_SHADER_TYPE::MESH_TEXTURE));
	meshData.IASetBuffer();

	ConstantBuffer::UpdateConstBuffer(CB_TYPE::CB_MATERIAL, material->materialParam);
	ConstantBuffer::SetPSRegister(1, CB_TYPE::CB_MATERIAL);
	material->SetPSResources();

	meshData.Draw();
}
