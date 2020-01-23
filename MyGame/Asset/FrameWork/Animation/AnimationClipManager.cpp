#include "AnimationClipManager.h"
#include "../../DirectX/ImGui/imgui.h"
#include "../Common.h"

using namespace FrameWork;
using namespace Input;

static const char ANIMATIONCLIP[] = "ANIMATIONCLIP";

AnimationClipManager::AnimationClipManager()
{
}

AnimationClipManager::~AnimationClipManager()
{
	animationMap.clear();
	takeNodeMap.clear();
}

void AnimationClipManager::DrawImGui()
{
	int cnt = 0;
	for (auto & map : animationMap)
	{
		std::string id = std::to_string(cnt);

		BeginDragImGui(map.second, id);
		cnt++;
	}
}

void AnimationClipManager::BeginDragImGui(std::unique_ptr<AnimationClip> & animClip, std::string id)
{
	const std::string strId = "##AnimationClip" + id;

	ImGui::Bullet(); ImGui::SameLine();
	ImGui::Selectable((animClip->name + strId).c_str());

	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
	{
		ImGui::SetDragDropPayload(ANIMATIONCLIP, &animClip, sizeof(animClip));

		ImGui::Text(animClip->name.c_str());

		ImGui::EndDragDropSource();
	}
}

bool AnimationClipManager::DropTargetImGui(std::shared_ptr<AnimationClip>& animClip, std::string id)
{
	static std::shared_ptr<AnimationClip> * removeAnim = nullptr;
	const std::string strId = "##AnimationClip" + id;
	const std::string popUpName = "AnimationClip" + strId;
	ImGui::AlignTextToFramePadding();

	//ImGui::Text("AnimClip"); ImGui::SameLine();

	float width = ImGui::GetWindowWidth() - ImGui::GetCursorPosX();

	// ÉfÅ[É^Ç™Ç»Ç¢Ç∆Ç´
	if (!animClip)
	{
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.2f, 0.2f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.4f, 0.4f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
		ImGui::Button(("None" + strId).c_str(), ImVec2(width, ImGui::GetFrameHeight()));
		ImGui::PopStyleColor(4);
	}
	else
	{
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.4f, 1.0f, 0.4f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.6f, 1.0f, 0.6f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 1.0f, 0.2f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
		ImGui::Button((animClip->name + strId).c_str(), ImVec2(width, ImGui::GetFrameHeight()));
		ImGui::PopStyleColor(4);

		if (ImGui::IsItemHovered())
		{
			if (Mouse::IsTrigger(Mouse::Button::Right))
			{
				removeAnim = &animClip;
				ImGui::OpenPopup(popUpName.c_str());
			}
		}
	}

	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(ANIMATIONCLIP))
		{
			auto & payload_n = *(std::unique_ptr<AnimationClip>*)payload->Data;
			if (payload_n)
			{
				animClip = std::shared_ptr<AnimationClip>(payload_n->CreateClone());
				return true;
			}
		}
	}

	if (ImGui::BeginPopup(popUpName.c_str()))
	{
		if (removeAnim != nullptr && (*removeAnim))
		{
			if (ImGui::MenuItem("Remove"))
			{
				(*removeAnim).reset();
				removeAnim = nullptr;
			}
		}

		ImGui::EndPopup();
	}

	
	return false;
}

void AnimationClipManager::RegisterAnimation(Skeleton * skeleton)
{
	for (unsigned int a = 0; a < skeleton->animInfo.size(); a++)
	{
		std::string animName = skeleton->animInfo[a].name;

		// ìØÇ∂ÇæÇ¡ÇΩèÍçáÇÕì«Ç›çûÇ‹Ç»Ç¢
		if (IsRegistered(animName)) continue;

		auto & takeNode = takeNodeMap[animName] = std::make_shared<TakeNode>();
		auto & anim = animationMap[animName] = std::make_unique<AnimationClip>();

		anim->Initialize(animName, takeNode);
		anim->SetMaxFrame((int)skeleton->animInfo[a].maxFrame);
		anim->SetSpeed(60.0f);
		anim->SetLoop(true);

		for (auto & skin : skeleton->mJoints)
		{
			if (skin.animations.size() < a + 1) continue;

			for (auto & key : skin.animations[a].keyFrames)
			{
				anim->SetAnimationTake(&skin.hierarchy, key.frameNum, key.localMatrix);
			}
		}

	}
}

void AnimationClipManager::DeleteAnimation(Skeleton * skeleton)
{
	for (unsigned int a = 0; a < skeleton->animInfo.size(); a++)
	{
		std::string animName = skeleton->animInfo[a].name;
		
		for (auto itr = animationMap.begin(), end = animationMap.end(); itr != end; ++itr)
		{
			if (itr->first == animName)
			{
				animationMap.erase(itr);
				break;
			}
		}
		for (auto itr = takeNodeMap.begin(), end = takeNodeMap.end(); itr != end; ++itr)
		{
			if (itr->first == animName)
			{
				takeNodeMap.erase(itr);
				break;
			}
		}
	}
}

AnimationClip * AnimationClipManager::GetClone(std::string animName)
{
	if (animationMap.count(animName) == 0)
		return nullptr;

	return animationMap[animName]->CreateClone();
}

std::weak_ptr<TakeNode> AnimationClipManager::GetTakeNode(std::string name)
{
	if(takeNodeMap.count(name) == 0)
		return std::weak_ptr<TakeNode>();

	return takeNodeMap[name];
}

bool AnimationClipManager::IsRegistered(std::string animName)
{
	return animationMap.count(animName) > 0;
}
