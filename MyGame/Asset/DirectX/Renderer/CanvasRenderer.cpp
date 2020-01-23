#include "CanvasRenderer.h"
#include "../Texture/TextureManager.h"

using namespace MyDirectX;

CanvasRenderer::CanvasRenderer()
{
	model = std::make_shared<PlaneMesh>();
}

CanvasRenderer::CanvasRenderer(std::string imageName)
{
	model = std::make_shared<PlaneMesh>();
	model->material->pTexture = Singleton<TextureManager>::Instance()->Load(imageName);
}

CanvasRenderer::~CanvasRenderer()
{
}

void CanvasRenderer::Draw()
{
	RendererSystem::SetUseLightOption(false, false);

	// ベースカラーをピクセルシェーダーにセット
	ConstantBuffer::UpdateConstBuffer(CB_TYPE::CB_COLOR, baseColor);
	ConstantBuffer::SetPSRegister(0, CB_TYPE::CB_COLOR);

	Vector3 pos = transform.lock()->GetWorldPosition();
	Vector3 sca = transform.lock()->GetLocalScale();
	model->CanvasDraw(pos.x, pos.x + sca.x, pos.y, pos.y + sca.y);
}
