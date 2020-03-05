#ifndef _CONSTANT_BUFFER_H_
#define _CONSTANT_BUFFER_H_

#include "FrameWork/Common.h"
#include "../Renderer/System/RendererSystem.h"

using namespace FrameWork;

// �R���X�^���g�o�b�t�@�̓L�����悭�Ȃ��ƍ��Ȃ� (16�o�C�g��)

namespace MyDirectX
{
#define BONE_MAX (256)

	// �J�������
	struct CB_CAMERA
	{
		XMFLOAT4 eyePos;
		XMFLOAT4 diretion;
	};

	// �������̏��
	struct CB_DIRECTIONAL_LIGHT
	{
		XMFLOAT4	direction;
		Color		diffuse;
		Color		ambient;
	};

	struct CB_LIGHT_VIEW_PROJECTION
	{
		Matrix4 viewMatrix;
		Matrix4 projectionMatrix;
	};

	// ���C�g���g�p���邩���Ȃ���
	struct CB_USE_LIGHT
	{
		int useLight;
		int useCastShadow;
		int dumy[2];
	};

	// �{�[�����
	struct CB_ARMATURE
	{
		Matrix4 boneArray[BONE_MAX];
	};

	// �}�e���A�����
	struct CB_MATERIAL {
		Color ambient = Color(0.0f, 0.0f, 0.0f, 1.0f);
		Color diffuse = Color(1.0f, 1.0f, 1.0f, 1.0f);
		Color emissive = Color(0.0f, 0.0f, 0.0f, 1.0f);
		Color bump = Color(1.0f, 1.0f, 1.0f, 1.0f);

		Color specular = Color(0.25f, 0.25f, 0.25f, 1.0f);
		float specularFactor = 1.0f;

		float shininess = 1.0f;
		float reflection = 0.3f;

		float height = 0.0f;

	private:
		friend cereal::access;
		template <typename Archive>
		void serialize(Archive & archive, std::uint32_t const version)
		{
			archive(CEREAL_NVP(ambient), CEREAL_NVP(diffuse), CEREAL_NVP(emissive), CEREAL_NVP(bump), CEREAL_NVP(specular),
				CEREAL_NVP(specularFactor), CEREAL_NVP(shininess), CEREAL_NVP(reflection), CEREAL_NVP(height));
		}
	};

	struct CB_MATRIX_2
	{
		Matrix4 matrix1;
		Matrix4 matrix2;
	};

	struct CB_LINE_WIDTH
	{
		float Thickness;	 // ���C���̑���
		Vector3 CameraPos;
	};

	struct CB_FAR
	{
		Matrix4 world;
		UINT maxNum;		// �`���
		float scaleOffset;  // �@�������Ɋg�債�Ă����Ԋu
		float dumy[2];
	};

	enum class CB_TYPE : int
	{
		CB_WORLD,
		CB_VIEW,
		CB_PROJECTION,
		CB_ARMATURE,
		CB_COLOR,
		CB_MATERIAL,
		CB_CAMERA,
		CB_USE_LIGHT,
		CB_DIRECTIONAL_LIGHT,
		CB_LIGHT_VIEW_PROJECTION,
		CB_MATRIX_2,
		CB_LINE_WIDTH,
		CB_FAR,
		CB_NUM
	};

	class ConstantBuffer
	{
	public:

		static void Create();
		static void Release();

		// �R���X�^���g�o�b�t�@�̍X�V
		template<typename CB>
		static void UpdateConstBuffer(CB_TYPE type, const CB & cb)
		{
			RendererSystem::GetDeviceContext()->UpdateSubresource(constantBuffers[(int)type], 0, NULL, &cb, 0, 0);
		}

		// ���_�V�F�[�_�[�ɃR���X�^���g�o�b�t�@���Z�b�g
		static void SetVSRegister(UINT registerNum, CB_TYPE type)
		{
			RendererSystem::GetDeviceContext()->VSSetConstantBuffers(registerNum, 1, &constantBuffers[(int)type]);
		}

		// �W�I���g���V�F�[�_�[�ɃR���X�^���g�o�b�t�@���Z�b�g
		static void SetGSRegister(UINT registerNum, CB_TYPE type)
		{
			RendererSystem::GetDeviceContext()->GSSetConstantBuffers(registerNum, 1, &constantBuffers[(int)type]);
		}

		// �s�N�Z���V�F�[�_�[�ɃR���X�^���g�o�b�t�@���Z�b�g
		static void SetPSRegister(UINT registerNum, CB_TYPE type)
		{
			RendererSystem::GetDeviceContext()->PSSetConstantBuffers(registerNum, 1, &constantBuffers[(int)type]);
		}

	private:

		// �R���X�^���g�o�b�t�@�̐���
		template<typename CB>
		static void CreateConstBuffer(ID3D11Buffer** constBuf)
		{
			D3D11_BUFFER_DESC desc;
			ZeroMemory(&desc, sizeof(desc));
			desc.ByteWidth = sizeof(CB);
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			desc.CPUAccessFlags = 0;
			desc.MiscFlags = 0;
			desc.StructureByteStride = 0;
			RendererSystem::GetDevice()->CreateBuffer(&desc, NULL, constBuf);
		}

		// �R���X�^���g�o�b�t�@�̍X�V
		template<typename CB>
		static void UpdateDynamicConstBuffer(ID3D11Buffer* constBuffer, CB * cb)
		{
			D3D11_MAPPED_SUBRESOURCE pdata;
			RendererSystem::GetDeviceContext()->Map(constBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &pdata);
			memcpy_s(pdata.pData, pdata.RowPitch, (void*)(cb), sizeof(*cb));
			RendererSystem::GetDeviceContext()->Unmap(constBuffer, 0);
		}

		static ID3D11Buffer* constantBuffers[(int)CB_TYPE::CB_NUM];
	};
}

CEREAL_CLASS_VERSION(MyDirectX::CB_MATERIAL, 0);

#endif // !_CONSTANT_BUFFER_H_

