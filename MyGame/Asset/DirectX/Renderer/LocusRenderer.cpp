#include "LocusRenderer.h"

using namespace MyDirectX;

LocusRenderer::~LocusRenderer()
{
}

void LocusRenderer::DrawImGui(int id)
{
	std::string meshId = "##" + std::to_string(id);

	ImGui::ColorEdit4(("BaseColor" + meshId).c_str(), baseColor);

	ImGui::Text("Material");
	Singleton<MaterialManager>::Instance()->DropTargetImGuiMaterial(material, meshId);
}

void LocusRenderer::Awake()
{
	indexNum = 0;
	startNum = 0;
}

void LocusRenderer::CreateMesh(int frameMax)
{
	if (frameMax <= 0) return;

	std::vector<VTX_MESH> vertex(frameMax * 2);
	std::vector<unsigned short> index(frameMax * 2);
	for (int i = 0; i < frameMax; i++)
	{
		int vtx1 = i * 2;
		int vtx2 = i * 2 + 1;

		float u1 = Mathf::Lerp(0.0f, 1.0f, (float)vtx1 / (float)frameMax);
		float v1 = 0.0f;
		float u2 = u1;
		float v2 = 1.0f;

		vertex[vtx1].uv = Vector2(u1, v1);
		vertex[vtx2].uv = Vector2(u2, v2);
		index[vtx1] = vtx1;
		index[vtx2] = vtx2;
	}

	meshData.CreateVertexBuffer(&vertex);
	meshData.CreateIndexBuffer(&index);
}

void LocusRenderer::SetMesh(const std::vector<Vector3>& pos1, const std::vector<Vector3>& pos2, int startFrame, int endFrame)
{
	if (meshData.vertex.size() <= endFrame * 2) 
		return;

	startNum = startFrame * 2;
	indexNum = endFrame * 2;

	for (int i = startFrame; i < endFrame; i++)
	{
		int num = i * 2;
		meshData.vertex[num].pos = pos1[i];
		meshData.vertex[num + 1].pos = pos2[i];
	}
}

void LocusRenderer::Draw()
{
	if (material.expired() || indexNum <= 0 || meshData.vertex.size() == 0) return;

	// �p�����[�^�̎󂯓n��
	ConstantBuffer::UpdateConstBuffer(CB_TYPE::CB_WORLD, transform.lock()->GetWorldMatrix());
	ConstantBuffer::SetVSRegister(0, CB_TYPE::CB_WORLD);

	RendererSystem::GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	meshData.SetVertexBuffer();
	meshData.IASetBuffer();
	material.lock()->SetOption();

	RendererSystem::SetUseLightOption(false, false);

	// �x�[�X�J���[���s�N�Z���V�F�[�_�[�ɃZ�b�g
	ConstantBuffer::UpdateConstBuffer(CB_TYPE::CB_COLOR, baseColor);
	ConstantBuffer::SetPSRegister(0, CB_TYPE::CB_COLOR);

	// �`��
	meshData.DrawIndexed(indexNum - startNum, startNum);
}
