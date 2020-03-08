#include "SceneChangePoint.h"
#include "Actor/Player/PlayerActor.h"

void SceneChangePoint::DrawImGui(int id)
{
	std::string strId = "##SceneChangePoint" + std::to_string(id);
	MyImGui::InputString("NextSceneName" + strId, nextSceneName, 32);
	ImGui::InputInt(("EntryNumber" + strId).c_str(), &entryNumber);
	ImGui::Text("Scene Start Point");
	MyImGui::DropTargetComponent(sceneStartPoint, strId);
}

void SceneChangePoint::OnTriggerStay(std::weak_ptr<Collision>& mine, std::weak_ptr<Collision>& other)
{
	if (other.lock()->isTrigger) return;

	auto player = other.lock()->gameObject.lock()->GetComponent<PlayerActor>();
	if (player.expired()) return;

	// �v���C���[�ƐڐG������V�[����؂�ւ�
	if (PlaySceneChange != nullptr)
	{
		PlaySceneChange(*this);
	}
}
