#include "DXCamera.h"
#include "../Shader/ConstantBuffer.h"

using namespace FrameWork;

DXCamera::DXCamera()
{
	viewport = Rect(0, SCREEN_WIDTH, 0, SCREEN_HEIGHT);
	this->nearDistance = 0.1f;
	this->farDistance = 5000.0f;
}

DXCamera::DXCamera(int left, int right, int top, int bottom, float nearDistance, float farDistance)
{
	viewport = Rect(left, right, top, bottom);
	this->nearDistance = nearDistance;
	this->farDistance = farDistance;
}

void DXCamera::DrawImGui(int id)
{
	std::string strId = "##DXCamera" + std::to_string(id);
	ImGui::DragInt(("left" + strId).c_str(), &viewport.left);
	ImGui::DragInt(("right" + strId).c_str(), &viewport.right);
	ImGui::DragInt(("top" + strId).c_str(), &viewport.top);
	ImGui::DragInt(("bottom" + strId).c_str(), &viewport.bottom);

	ImGui::DragFloat(("near" + strId).c_str(), &nearDistance);
	ImGui::DragFloat(("far" + strId).c_str(), &farDistance);
}

bool DXCamera::IsVisiblity(Vector3 position)
{
	/*Vector3 viewPos, porjectionPos;
	
	Matrix4 viewPro = projectionMatrix * viewMatrix;
	porjectionPos = viewPro * position;*/

	XMVECTOR WorldPos, ViewPos, ProjectPos;
	XMFLOAT3 porjectionPos;
	XMMATRIX4 view = viewMatrix;
	XMMATRIX4 projection = projectionMatrix;
	// 座標変換
	WorldPos = XMLoadFloat3(&position.XMFLOAT());
	ViewPos = XMVector3TransformCoord(WorldPos, view.matrix);
	ProjectPos = XMVector3TransformCoord(ViewPos, projection.matrix);
	XMStoreFloat3(&porjectionPos, ProjectPos);
	
	if (-1.0f < porjectionPos.x && porjectionPos.x < 1.0f &&
		-1.0f < porjectionPos.y && porjectionPos.y < 1.0f &&
		0.0f < porjectionPos.z && porjectionPos.z < 1.0f)
	{
		return true;
	}

	return false;
}

void DXCamera::Draw()
{
	float viewWidth = (float)(viewport.right - viewport.left);
	float viewHeight = (float)(viewport.bottom - viewport.top);

	// ビューポート設定
	D3D11_VIEWPORT dxViewport;
	dxViewport.TopLeftX = (float)viewport.left;
	dxViewport.TopLeftY = (float)viewport.top;
	dxViewport.Width = viewWidth;
	dxViewport.Height = viewHeight;
	dxViewport.MinDepth = 0.0f;
	dxViewport.MaxDepth = 1.0f;
	RendererSystem::GetDeviceContext()->RSSetViewports(1, &dxViewport);

	// ビューマトリクス設定
	Matrix4 cameraMatrix(transform.lock()->GetWorldPosition(), Vector3::one(), transform.lock()->GetWorldRotation());
	viewMatrix = cameraMatrix.Inverse();

	// カメラのマトリクスをセット( データ取得用 )
	RendererSystem::SetCameraMatrix(cameraMatrix);
	RendererSystem::SetViewMatrix(viewMatrix);

	// プロジェクションマトリクス設定
	projectionMatrix = XMMatrixPerspectiveFovLH(1.0f, dxViewport.Width / dxViewport.Height, nearDistance, farDistance);
	RendererSystem::SetProjectionMatrix(projectionMatrix);

	// コンスタントバッファに登録
	CB_CAMERA cb;
	Vector3 pos = transform.lock()->GetWorldPosition();
	Vector3 dir = transform.lock()->forward();
	cb.eyePos = XMFLOAT4(pos.x, pos.y, pos.z, 1.0f);
	cb.diretion = XMFLOAT4(dir.x, dir.y, dir.z, 1.0f);

	ConstantBuffer::UpdateConstBuffer(CB_TYPE::CB_CAMERA, cb);
	ConstantBuffer::SetVSRegister(5, CB_TYPE::CB_CAMERA);
	ConstantBuffer::SetPSRegister(2, CB_TYPE::CB_CAMERA);
}

void DXCamera::ScreenToWorldPoint(Vector3 & outPos, const Vector2 & screenPos, float depth)
{
	// 各行列の逆行列を算出
	XMMATRIX4 invView, invPrj, vp, invViewport;
	invView = viewMatrix;
	invPrj = projectionMatrix;
	invView = XMMatrixInverse(NULL, invView.matrix);
	invPrj = XMMatrixInverse(NULL, invPrj.matrix);

	float viewWidth = (float)(viewport.right - viewport.left);
	float viewHeight = (float)(viewport.bottom - viewport.top);

	vp = XMMatrixIdentity();
	float screenW, screenH;
	screenW = viewWidth;
	screenH = viewHeight;
	vp.matrix.r[0].m128_f32[0] = screenW / 2.0f; vp.matrix.r[1].m128_f32[1] = -screenH / 2.0f;
	vp.matrix.r[3].m128_f32[0] = screenW / 2.0f; vp.matrix.r[3].m128_f32[1] = screenH / 2.0f;
	//vp.matrix.r[2].m128_f32[2] = 1.0f; vp.matrix.r[3].m128_f32[3] = 1.0f;
	invViewport = XMMatrixInverse(NULL, vp.matrix);

	// 逆変換
	XMMATRIX tmp = invViewport.matrix * invPrj.matrix * invView.matrix;
	XMFLOAT3 pos = XMFLOAT3(screenPos.x, screenPos.y, depth);
	XMVECTOR xmVec = XMLoadFloat3(&pos);
	xmVec = XMVector3TransformCoord(xmVec, tmp);
	XMFLOAT3 xmOutPos;
	XMStoreFloat3(&xmOutPos, xmVec);
	outPos = Vector3(xmOutPos.x, xmOutPos.y, xmOutPos.z);
}
