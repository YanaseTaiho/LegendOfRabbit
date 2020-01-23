#include "RotationFixedController.h"

void RotationFixedController::DrawImGui(int id)
{
	std::string strId = "##" + std::to_string(id);

	MyImGui::VectorEdit<FixedRotation>("RotationArray", id, fixedRotationArray, [&](FixedRotation& v, int cnt)
	{
		strId = "##" + std::to_string(cnt);
		MyImGui::DropTargetComponent(v.transform, strId);
		if (v.transform.expired()) return;

		Vector3 rot = v.rotation.GetEulerAngles();
		if (ImGui::DragFloat3(("Rotation" + strId).c_str(), rot, 0.05f))
		{
			v.rotation = Quaternion::EulerAngles(rot);
		}
	});
}

void RotationFixedController::LateUpdate()
{
	if (weightValue > 0.0f || weightValue < 1.0f)
	{
		weightValue += weightSpeed;
		weightValue = Mathf::Clamp01(weightValue);
	}

	for (auto v : fixedRotationArray)
	{
		if (v.transform.expired())continue;
		Quaternion q = v.transform.lock()->GetLocalRotation();
		v.transform.lock()->SetLocalRotation(q.Slerp(v.rotation, weightValue));
	}
}
