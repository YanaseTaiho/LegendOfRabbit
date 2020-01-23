#include "MonoBehaviour.h"
#include "../../GameObject/GameObject.h"
#include "../../Scene/SceneManager.h"
#include "../../Scene/SceneBase.h"

std::list<std::weak_ptr<MonoBehaviour>> MonoBehaviour::behaviourList;

void MonoBehaviour::LoadSerialized(const std::shared_ptr<Component>& component)
{
	Component::LoadSerialized(component);
	AddComponentList(behaviourList);

	// シーンが既に開始していたら
	if (Singleton<SceneManager>::Instance()->GetCurrentScene()->IsStart())
		this->Start();
}

void MonoBehaviour::AddComponent()
{
	Component::AddComponent();
	AddComponentList(behaviourList);
	auto & com = behaviourList.back();
	gameObject.lock()->SetMonoBehaviour(com);

	// シーンが既に開始していたら
	if (Singleton<SceneManager>::Instance()->GetCurrentScene()->IsStart())
		com.lock()->Start();
}

void MonoBehaviour::RemoveComponent()
{
	RemoveComponentList(behaviourList);
	// オブジェクトが削除されていない時
	if(!gameObject.expired()) gameObject.lock()->RemoveMonoBehaviour(this);
	Component::RemoveComponent();
}
