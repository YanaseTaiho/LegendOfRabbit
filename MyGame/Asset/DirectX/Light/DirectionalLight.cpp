#include "DirectionalLight.h"
#include "../Shadow/Shadow.h"
#include "../Shader/ConstantBuffer.h"

#include "../MeshData/LineMesh/DebugLine.h"

using namespace FrameWork;
using namespace MyDirectX;

#define DEPTH_FAR (512.0f)

void DirectionalLight::Awake()
{
	// 正射影行列作成
	orthoMatrix = XMMatrixOrthographicLH(DEPTH_FAR, DEPTH_FAR, 1.0f, DEPTH_FAR);
}

void DirectionalLight::DrawImGui(int id)
{
	std::string strId = "##Light" + std::to_string(id);
	ImGui::ColorEdit3(("Diffuse"+ strId).c_str(), diffuse);
	ImGui::ColorEdit3(("Ambient"+ strId).c_str(), ambient);
}

void DirectionalLight::SetLight()
{
	// ライトの情報を定数バッファに登録
	CB_DIRECTIONAL_LIGHT cb;
	Vector3 dir = transform.lock()->forward();
	cb.direction = XMFLOAT4(dir.x, dir.y, dir.z, 1.0f);
	cb.diffuse = diffuse;
	cb.ambient = ambient;
	ConstantBuffer::UpdateConstBuffer(CB_TYPE::CB_DIRECTIONAL_LIGHT, cb);
	ConstantBuffer::SetVSRegister(6, CB_TYPE::CB_DIRECTIONAL_LIGHT);
	ConstantBuffer::SetPSRegister(4, CB_TYPE::CB_DIRECTIONAL_LIGHT);

	// ライトのビュー行列を定数バッファに登録
	CB_LIGHT_VIEW_PROJECTION cbVP;
	Vector3 cameraPos = RendererSystem::GetCameraMatrix().position() + RendererSystem::GetCameraMatrix().forward() * DEPTH_FAR * 0.3f;
	Vector3 lightForward = transform.lock()->forward();
	Vector3 pos = cameraPos - lightForward * DEPTH_FAR * 0.5f;
	Vector3 at = cameraPos + lightForward;
	Matrix4 lightView = XMMatrixLookAtLH({ pos.x,pos.y,pos.z,1.0f }, { at.x,at.y,at.z,1.0f }, { 0.0f,1.0f,0.0f,1.0f });
	cbVP.viewMatrix = lightView;
	cbVP.projectionMatrix = orthoMatrix;
	ConstantBuffer::UpdateConstBuffer(CB_TYPE::CB_LIGHT_VIEW_PROJECTION, cbVP);
	ConstantBuffer::SetVSRegister(4, CB_TYPE::CB_LIGHT_VIEW_PROJECTION);

	DebugLine::DrawLine("Sphere", transform.lock()->GetWorldMatrix(), Color::yellow(), 0.1f);
	DebugLine::DrawRay(transform.lock()->GetWorldPosition(), transform.lock()->forward(), 5.0f, Color::yellow(), 0.1f);
}
