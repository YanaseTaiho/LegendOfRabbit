#include "TargetImageController.h"

void TargetImageController::DrawImGui(int id)
{
	std::string strId = "##TargetImageController" + std::to_string(id);

	ImGui::DragFloat(("TargetScale" + strId).c_str(), &targetScale, 0.1f);
	ImGui::DragFloat(("RotationSpeed" + strId).c_str(), &rotationSpeed, 0.1f);
}

void TargetImageController::TargetStart()
{
	transform.lock()->SetWorldScale(Vector3::one() * targetScale * 15.0f);
}

void TargetImageController::Start()
{
	renderer = this->gameObject.lock()->GetComponent<BillboardRenderer>();
}

void TargetImageController::LateUpdate()
{
	if (renderer.expired()) return;

	Vector3 scale = Vector3::Lerp(transform.lock()->GetWorldScale(), Vector3::one() * targetScale, 10.0f * Time::DeltaTime());

	transform.lock()->SetWorldScale(scale);
	renderer.lock()->rotation += rotationSpeed * Time::DeltaTime();
}
