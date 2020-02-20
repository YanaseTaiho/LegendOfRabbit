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
	// トランスフォーム更新
	EffekseerManager* manager = Singleton<EffekseerManager>::Instance();
	Effekseer::Matrix43 setMat;
	Matrix4 matrix = transform.lock()->GetWorldMatrix();
	setMat.Value[0][0] = matrix.matrix(0, 0);
	setMat.Value[0][1] = matrix.matrix(1, 0);
	setMat.Value[0][2] = matrix.matrix(2, 0);

	setMat.Value[1][0] = matrix.matrix(0, 1);
	setMat.Value[1][1] = matrix.matrix(1, 1);
	setMat.Value[1][2] = matrix.matrix(2, 1);

	setMat.Value[2][0] = matrix.matrix(0, 2);
	setMat.Value[2][1] = matrix.matrix(1, 2);
	setMat.Value[2][2] = matrix.matrix(2, 2);

	setMat.Value[3][0] = matrix.matrix(0, 3);
	setMat.Value[3][1] = matrix.matrix(1, 3);
	setMat.Value[3][2] = matrix.matrix(2, 3);

	manager->manager->SetMatrix(handle, setMat);
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
