#include "ConstantBuffer.h"

using namespace MyDirectX;

ID3D11Buffer* ConstantBuffer::constantBuffers[(int)CB_TYPE::CB_NUM] = { nullptr };

void ConstantBuffer::Create()
{
	CreateConstBuffer<Matrix4>(&constantBuffers[(int)CB_TYPE::CB_WORLD]);
	CreateConstBuffer<Matrix4>(&constantBuffers[(int)CB_TYPE::CB_VIEW]);
	CreateConstBuffer<Matrix4>(&constantBuffers[(int)CB_TYPE::CB_PROJECTION]);
	CreateConstBuffer<CB_ARMATURE>(&constantBuffers[(int)CB_TYPE::CB_ARMATURE]);
	CreateConstBuffer<Color>(&constantBuffers[(int)CB_TYPE::CB_COLOR]);
	CreateConstBuffer<CB_MATERIAL>(&constantBuffers[(int)CB_TYPE::CB_MATERIAL]);
	CreateConstBuffer<CB_CAMERA>(&constantBuffers[(int)CB_TYPE::CB_CAMERA]);
	CreateConstBuffer<CB_USE_LIGHT>(&constantBuffers[(int)CB_TYPE::CB_USE_LIGHT]);
	CreateConstBuffer<CB_DIRECTIONAL_LIGHT>(&constantBuffers[(int)CB_TYPE::CB_DIRECTIONAL_LIGHT]);
	CreateConstBuffer<CB_LIGHT_VIEW_PROJECTION>(&constantBuffers[(int)CB_TYPE::CB_LIGHT_VIEW_PROJECTION]);
	CreateConstBuffer<CB_LINE_WIDTH>(&constantBuffers[(int)CB_TYPE::CB_LINE_WIDTH]);
}

void ConstantBuffer::Release()
{
	for (int i = 0; i < (int)CB_TYPE::CB_NUM; i++)
	{
		if(constantBuffers[i])
			constantBuffers[i]->Release();
	}
}
