#include "MonoBehaviour.h"
#include "../../GameObject/GameObject.h"
#include "../../Scene/SceneManager.h"
#include "../../Scene/SceneBase.h"

std::list<std::weak_ptr<MonoBehaviour>> MonoBehaviour::behaviourList;

void MonoBehaviour::LoadSerialized(const std::shared_ptr<Component>& component)
{
	Component::LoadSerialized(component);
	AddComponentList(behaviourList);

	// �V�[�������ɊJ�n���Ă�����
	if (Singleton<SceneManager>::Instance()->GetCurrentScene()->IsStart())
		this->Start();
}

void MonoBehaviour::AddComponent()
{
	Component::AddComponent();
	AddComponentList(behaviourList);
	auto & com = behaviourList.back();
	gameObject.lock()->SetMonoBehaviour(com);

	// �V�[�������ɊJ�n���Ă�����
	if (Singleton<SceneManager>::Instance()->GetCurrentScene()->IsStart())
		com.lock()->Start();
}

void MonoBehaviour::RemoveComponent()
{
	RemoveComponentList(behaviourList);
	// �I�u�W�F�N�g���폜����Ă��Ȃ���
	if(!gameObject.expired()) gameObject.lock()->RemoveMonoBehaviour(this);
	Component::RemoveComponent();
}
