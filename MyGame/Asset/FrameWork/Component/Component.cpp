#include "Component.h"
#include "../GameObject/GameObject.h"
#include "Transform/Transform.h"

using namespace FrameWork;

std::list<std::shared_ptr<Component>> Component::allComponentList;
std::list<std::weak_ptr<Component>> Component::destroyComponentList;

std::weak_ptr<Component> Component::AddComponent(Component * component, std::weak_ptr<GameObject>& object, std::weak_ptr<Transform>& transform)
{
	component->Initialize(object, transform);
	allComponentList.emplace_back(std::shared_ptr<Component>(component));
	auto & com = allComponentList.back();
	component->AddComponent();
	return com;
}

void Component::RemoveComponent(std::weak_ptr<Component>& component)
{
	for (auto itr = allComponentList.begin(), end = allComponentList.end(); itr != end; ++itr)
	{
		if (*itr == component.lock())
		{
			destroyComponentList.emplace_back(*itr);
			return;
		}
	}
}

void Component::Initialize(std::weak_ptr<GameObject> & object, std::weak_ptr<Transform> & transform)
{
	this->gameObject = object;
	this->transform = transform;
}

void Component::RemoveComponent()
{
	if (gameObject.expired()) return;
	gameObject.lock()->RemoveComponentList(this);
}

void Component::CleanupDestoryComponent()
{
	for (auto deth : destroyComponentList)
	{
		deth.lock()->OnDestroy();
	}

	for (auto itr = destroyComponentList.begin(), end = destroyComponentList.end(); itr != end;)
	{
		for (auto comItr = allComponentList.begin(), comEnd = allComponentList.end(); comItr != comEnd; ++comItr)
		{
			if (itr->lock() != *comItr) continue;

			itr = destroyComponentList.erase(itr);
			
			(*comItr)->RemoveComponent();
			allComponentList.erase(comItr);
			break;
		}
	}
}

bool Component::IsEnable()
{
	if (gameObject.expired()) return enable;

	return enable && gameObject.lock()->IsActive(); 
}

//// 非侵入型のシリアライズ定義
//template<class Archive>
//void serialize(Archive & archive, Component &component)
//{
//	archive(CEREAL_NVP(component.enable), CEREAL_NVP(component.gameObject), CEREAL_NVP(component.transform));
//}