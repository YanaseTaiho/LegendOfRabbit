#include "SkinMesh.h"
#include "FrameWork/Animation/AnimationClipManager.h"
#include "../../Shader/ConstantBuffer.h"
#include "../../Shader/StandardShader.h"

using namespace MyDirectX;

#include "../../Texture/TextureManager.h"

void SkinMesh::Initialize()
{
	// �A�j���[�V�����f�[�^�̐���
	Singleton<AnimationClipManager>::Instance()->RegisterAnimation(skeleton.get());

	// �V�F�[�_�[�Z�b�g
	for (auto & node : meshNode)
	{
		for (auto& subset : node.subsetArray)
		{
			subset.material.SetShader(new StandardShader());
		}
	}
}

void SkinMesh::Draw(Transform * transform, std::vector<std::weak_ptr<Material>> & materialArray, const std::vector<std::weak_ptr<Transform>> & bones)
{
	// �p�����[�^�̎󂯓n��
	static CB_ARMATURE cbArm;

	RendererSystem::GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// �{�[���̍s����Z�b�g
	for (UINT i = 0, size = (UINT)boneData.size();i < size;i++)
	{
		/// �V�F�[�_�[�ɂ͕K���]�u���Ă���n��
		// Eigen�̓V�F�[�_�[�Ɠ������я��Ȃ̂ł��̂܂ܓn��
		cbArm.boneArray[i].matrix = bones[i].lock()->GetWorldMatrix().matrix * boneData[i].offsetMatrix.matrix;
	}

	ConstantBuffer::UpdateConstBuffer(CB_TYPE::CB_WORLD, transform->GetWorldMatrix());
	ConstantBuffer::UpdateConstBuffer(CB_TYPE::CB_ARMATURE, cbArm);

	ConstantBuffer::SetVSRegister(0, CB_TYPE::CB_WORLD);
	ConstantBuffer::SetVSRegister(3, CB_TYPE::CB_ARMATURE);

	int materialCnt = 0;

	for (auto & node : meshNode)
	{
		MeshData<VTX_SKIN_MESH> & meshData = (MeshData<VTX_SKIN_MESH>&)node.meshData;
		meshData.IASetBuffer();

		for (const auto& subset : node.subsetArray)
		{
			const Material * material;
			if (!materialArray[materialCnt].expired())
				material = materialArray[materialCnt].lock().get();
			else
				material = &subset.material;

			// �`��
			if (material->shader)
			{
				// ���X�^���C�U�Z�b�g
				RendererSystem::SetRasterizerState(material->rasterizer);
				// �V�F�[�_�[�Z�b�g
				//material->shader->SetShader();
				// �����ŕ`��
				material->shader->Draw(transform, material, &meshData, subset.startIndex, subset.indexNum);
			}
			
			//meshData.DrawIndexed(subset.indexNum, subset.startIndex);

			materialCnt++;
		}
	}
}

void SkinMesh::DrawShadow(Transform * transform, const std::vector<std::weak_ptr<Transform>>& bones)
{
	// �p�����[�^�̎󂯓n��
	CB_ARMATURE cbArm;

	RendererSystem::GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// �{�[���̍s����Z�b�g
	for (UINT i = 0, size = (UINT)boneData.size(); i < size; i++)
	{
		/// �V�F�[�_�[�ɂ͕K���]�u���Ă���n��
		// Eigen�̓V�F�[�_�[�Ɠ������я��Ȃ̂ł��̂܂ܓn��
		cbArm.boneArray[i] = bones[i].lock()->GetWorldMatrix() * boneData[i].offsetMatrix;	// ��Ń��[���h�s��œ����
	}

	ConstantBuffer::UpdateConstBuffer(CB_TYPE::CB_WORLD, transform->GetWorldMatrix());
	ConstantBuffer::UpdateConstBuffer(CB_TYPE::CB_ARMATURE, cbArm);

	ConstantBuffer::SetVSRegister(0, CB_TYPE::CB_WORLD);
	ConstantBuffer::SetVSRegister(3, CB_TYPE::CB_ARMATURE);

	for (auto & node : meshNode)
	{
		MeshData<VTX_SKIN_MESH> & meshData = (MeshData<VTX_SKIN_MESH>&)node.meshData;
		const auto & vertex = ShaderFormatManager::Instance()->GetVertexFormat(VERTEX_SHADER_TYPE::SKINMESH_SHADOW);
		const auto & pixel = ShaderFormatManager::Instance()->GetPixelFormat(PIXEL_SHADER_TYPE::SHADOW);
		// �[�x�e�N�X�`���p�̃V�F�[�_�[���Z�b�g
		Shader::SetShader(vertex, pixel);
		meshData.IASetBuffer();

		for (const auto& subset : node.subsetArray)
		{
			// �`��
			meshData.DrawIndexed(subset.indexNum, subset.startIndex);
		}
	}
}

int SkinMesh::GetMaterialNum()
{
	int num = 0;
	for (auto & mesh : meshNode)
	{
		for (auto & sub : mesh.subsetArray)
		{
			num++;
		}
	}
	return num;
}
