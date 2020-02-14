#include "EffekseerSystem.h"

using namespace MyDirectX;

void EffekseerSystem::DrawImGui(int id)
{
	std::string strId = "##EffekseerSystem" + std::to_string(id);

	ImGui::Text("Effect Type"); ImGui::SameLine();
	if (ImGui::Button(enum_to_string(type).c_str()))
	{
		ImGui::OpenPopup("EffekseerSystem##PopUp");
	}
	ImGui::Checkbox(("Has Start" + strId).c_str(), &hasStart);
	ImGui::Checkbox(("End Destroy" + strId).c_str(), &endDestroy);
	ImGui::Checkbox(("Loop" + strId).c_str(), &loop);

	if (ImGui::BeginPopup("EffekseerSystem##PopUp"))
	{
		for (int i = 0; i < (int)EffekseerType::MaxNum; i++)
		{
			if (ImGui::MenuItem(enum_to_string((EffekseerType)i).c_str()))
			{
				type = (EffekseerType)i;
			}
		}
		ImGui::EndPopup();
	}
}

void EffekseerSystem::Play()
{
	EffekseerManager* manager = Singleton<EffekseerManager>::Instance();
	
	// 実行中のエフェクトを削除
	if (handle >= 0)
	{
		manager->RemoveHandle(handle);
	}

	handle = manager->Play(type, gameObject.lock()->GetComponent<EffekseerSystem>());
	// トランスフォーム更新
	SetTransform();
}

void EffekseerSystem::Stop()
{
	if (handle < 0) return;

	EffekseerManager* manager = Singleton<EffekseerManager>::Instance();
	manager->manager->StopEffect(handle);
}

void EffekseerSystem::EndEffect()
{
	handle = -1;

	if (endDestroy)
	{
		GameObject::Destroy(this->gameObject);
	}
}

void EffekseerSystem::SetTransform()
{
	EffekseerManager* manager = Singleton<EffekseerManager>::Instance();
	// トランスフォーム更新
	Vector3 pos = transform.lock()->GetWorldPosition();
	manager->manager->SetLocation(handle, pos.x, pos.y, pos.z);
	Vector3 scale = transform.lock()->GetWorldScale();
	manager->manager->SetScale(handle, scale.x, scale.y, scale.z);
	Vector3 rot = transform.lock()->GetWorldRotation().GetEulerAnglesToRadian();
	manager->manager->SetRotation(handle, rot.x, rot.y, rot.z);
}

void EffekseerSystem::Start()
{
	EffekseerManager* manager = Singleton<EffekseerManager>::Instance();

	// 実行時エフェクトスタート
	if (hasStart)
	{
		handle = manager->Play(type, gameObject.lock()->GetComponent<EffekseerSystem>());
	}

	if (handle < 0) return;

	// トランスフォーム更新
	SetTransform();
}

void EffekseerSystem::LateUpdate()
{
	if (handle < 0)
	{
		// ループ再生ならマネージャーでエフェクト再生
		if (loop)
		{
			EffekseerManager* manager = Singleton<EffekseerManager>::Instance();
			handle = manager->Play(type, gameObject.lock()->GetComponent<EffekseerSystem>());
		}
		else return;
	}

	// トランスフォーム更新
	SetTransform();
}

void EffekseerSystem::OnDestroy()
{
	if (handle < 0) return;

	EffekseerManager* manager = Singleton<EffekseerManager>::Instance();
	manager->RemoveHandle(handle);
}
