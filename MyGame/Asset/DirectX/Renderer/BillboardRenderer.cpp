#include "BillboardRenderer.h"

using namespace MyDirectX;

void BillboardRenderer::DrawImGui(int id)
{
	std::string strId = "##BillboardRenderer" + std::to_string(id);
	if (Singleton<TextureManager>::Instance()->DropTargetImGuiTexture(texture, Vector2(50.0f, 50.0f), strId))
	{
		float w = (float)texture.lock()->GetWidth();
		float h = (float)texture.lock()->GetHeight();

		// 長さを 1 を最大とする
		if (w >= h){ h /= w; w = 1.0f;}
		else { w /= h; h = 1.0f;}

		float harfW = w * 0.5f;
		float harfH = h * 0.5f;

		rect.left = -harfW;
		rect.right = harfW;
		rect.top = -harfH;
		rect.bottom = harfH;
	}

	ImGui::Checkbox(("Depth Enable" + strId).c_str(), &isDepthEnable);
	ImGui::Separator();

	ImGui::DragFloat(("Rotation" + strId).c_str(), &rotation, 0.1f);
	ImGui::Separator();

	ImGui::DragFloat(("Left" + strId).c_str(), &rect.left, 0.001f);
	ImGui::DragFloat(("Right" + strId).c_str(), &rect.right, 0.001f);
	ImGui::DragFloat(("Top" + strId).c_str(), &rect.top, 0.001f);
	ImGui::DragFloat(("Bottom" + strId).c_str(), &rect.bottom, 0.001f);
	ImGui::Separator();

	ImGui::DragFloat(("U1" + strId).c_str(), &uv1.x, 0.01f);// ImGui::SameLine();
	ImGui::DragFloat(("V1" + strId).c_str(), &uv1.y, 0.01f);
	ImGui::DragFloat(("U2" + strId).c_str(), &uv2.x, 0.01f);// ImGui::SameLine();
	ImGui::DragFloat(("V2" + strId).c_str(), &uv2.y, 0.01f);
}

void BillboardRenderer::Draw()
{
	if (texture.expired()) return;

	// 常にカメラに向かうように設定
	auto & camera = RendererSystem::GetCameraMatrix();
	Vector3 lookAt = camera.position() - transform.lock()->GetWorldPosition();
	Quaternion q = Quaternion::LookRotation(lookAt, -camera.up()).Normalized();
	q = Quaternion::AxisAngle(lookAt.Normalize(), rotation).Normalized() * q;

	Matrix4 billbordMat(transform.lock()->GetWorldPosition(), transform.lock()->GetWorldScale(), q);
	ConstantBuffer::UpdateConstBuffer(CB_TYPE::CB_WORLD, billbordMat);
	ConstantBuffer::SetVSRegister(0, CB_TYPE::CB_WORLD);

	RendererSystem::SetUseLightOption(false, false);

	// ベースカラーをピクセルシェーダーにセット
	ConstantBuffer::UpdateConstBuffer(CB_TYPE::CB_COLOR, baseColor);
	ConstantBuffer::SetPSRegister(0, CB_TYPE::CB_COLOR);

	PlaneMesh::SetTexture(texture);					// テクスチャをセット
	PlaneMesh::SetUV(uv1.x, uv1.y, uv2.x, uv2.y);	// UVをセット

	// 深度値を使用するかどうか
	if(!isDepthEnable) RendererSystem::SetDepthEnable(isDepthEnable);

	// ビルボード専用の描画関数
	PlaneMesh::Draw(rect.left, rect.right, rect.top, rect.bottom);

	// 深度値を元に戻す
	RendererSystem::SetDepthEnable(true);
}
