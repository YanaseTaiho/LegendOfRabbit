#include "Animator.h"
#include "../Transform/Transform.h"
#include "../../Scene/SceneManager.h"
#include "../../Scene/SceneBase.h"
#include "../../Singleton.h"

using namespace FrameWork;

std::list<std::weak_ptr<Animator>> Animator::animatorList;

static BindClass<AnimationController> animControllerList;

static const char PopupMenuAnimController[] = "PopupMenuAnimController";

void Animator::CreateAnimationControllerList(BindClass<AnimationController> animControList)
{
	animControllerList = animControList;
}

void Animator::LoadSerialized(const std::shared_ptr<Component>& component)
{
	Component::LoadSerialized(component);
	AddComponentList(animatorList);

	this->animController->Initialize();

	// シーンが既に開始していたら
	if (Singleton<SceneManager>::Instance()->GetCurrentScene()->IsStart())
		this->Start();
}

void Animator::AddComponent()
{
	Component::AddComponent();
	AddComponentList(animatorList);

	// シーンが既に開始していたら
	if (Singleton<SceneManager>::Instance()->GetCurrentScene()->IsStart())
		this->Start();
}

void Animator::DrawImGui(int id)
{
	std::string strId = "##Animator" + std::to_string(id);
	std::string animControName = "None";
	if (animController)
		animControName = animControllerList.GetBindClassName(*animController.get());

	ImGui::Text("AnimationController");// ImGui::SameLine();

	float width = ImGui::GetWindowWidth() - ImGui::GetCursorPosX();
	if (ImGui::Button((animControName + strId).c_str(), ImVec2(width, ImGui::GetFrameHeight())))
	{
		ImGui::OpenPopup(PopupMenuAnimController);
	}

	if (ImGui::BeginPopup(PopupMenuAnimController))
	{
		ImGui::BeginChild("Child##Animator", ImVec2(250, 200), true);
		for (auto & data : animControllerList.list)
		{
			if (ImGui::MenuItem((data->name + strId).c_str()))
			{
				ImGui::CloseCurrentPopup();
				animController = std::shared_ptr<AnimationController>(data->instantiate());	// シェーダーのインスタンス生成
				animController->Initialize();
			}
		}
		ImGui::EndChild();

		ImGui::EndPopup();
	}

	if (animController)
	{
		animController->DrawimGui(id);
	}
}

Animator::Animator(AnimationController * animController)
{
	this->animController = std::shared_ptr<AnimationController>(animController);
	this->animController->Initialize();

}

Animator::~Animator()
{
}

void Animator::Start()
{
	if (!this->animController)return;

	this->animController->Start();
}

void Animator::Update()
{
	if (!this->animController)return;

	this->animController->Update(transform.lock().get());
	this->transform.lock()->UpdateChilds();
}

void Animator::SetAnimationCallBack(std::string name, int frame, std::function<void(void)> callBack)
{
	this->animController->SetAnimationCallBack(name, frame, callBack);
}

void Animator::SetFloat(std::string name, float value)
{
	this->animController->SetParameterFloat(name, value);
}

void Animator::SetInt(std::string name, int value)
{
	this->animController->SetParameterInt(name, value);
}

void Animator::SetBool(std::string name, bool value)
{
	this->animController->SetParameterBool(name, value);
}

void Animator::SetTrigger(std::string name, bool param)
{
	this->animController->SetParameterTrigger(name, param);
}

float Animator::GetFloat(std::string name)
{
	return this->animController->GetParameterFloat(name);
}

int Animator::GetInt(std::string name)
{
	return this->animController->GetParameterInt(name);
}

bool Animator::GetBool(std::string name)
{
	return this->animController->GetParameterBool(name);
}

bool Animator::GetTrigger(std::string name)
{
	return this->animController->GetParameterTrigger(name);
}

float Animator::GetCurrentPercent()
{
	return this->animController->GetCurrentPercent();
}

bool Animator::IsCurrentAnimation(std::string name)
{
	return this->animController->IsCurrentState(name);
}
