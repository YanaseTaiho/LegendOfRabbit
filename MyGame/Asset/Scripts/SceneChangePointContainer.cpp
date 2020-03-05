#include "SceneChangePointContainer.h"

void SceneChangePointContainer::DrawImGui(int id)
{
	std::string strId = "##SceneChangePointContainer" + std::to_string(id);
	MyImGui::VectorEdit<std::weak_ptr<SceneChangePoint>>("PointArray" + strId, id, pointArray, [&](std::weak_ptr<SceneChangePoint>& v, int cnt)
	{
		strId = "##" + std::to_string(cnt);
		MyImGui::DropTargetComponent(v, strId);
		if (v.expired()) return;

		v.lock()->DrawImGui(cnt);
	});
}
