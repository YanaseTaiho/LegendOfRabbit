#include "GameObject.h"
#include "../Common.h"

using namespace FrameWork;

void GameObject::DrawImGui()
{
	if (ImGui::RadioButton("Active", isActive))
		SetActive(!isActive);
//	ImGui::SameLine();

	char inName[32] = {};
	if (!name.empty())
		memcpy(inName, &name.front(), name.size());
	if (ImGui::InputText("Name", inName, ARRAYSIZE(inName)))
		name = inName;

	//int layerNum = (int)layer;
	ImGui::Text("Layer"); ImGui::SameLine();
	if (ImGui::Button(enum_to_string(layer).c_str(), ImVec2(100.0f, 20.0f)))
	{
		ImGui::OpenPopup("Layer##PopUp");
	}
	ImGui::SameLine();
	//int tagNum = (int)tag;
	ImGui::Text("Tag"); ImGui::SameLine();
	if (ImGui::Button(enum_to_string(tag).c_str(), ImVec2(100.0f, 20.0f)))
	{
		ImGui::OpenPopup("Tag##PopUp");
	}

	if(ImGui::BeginPopup("Layer##PopUp"))
	{
		for (int i = 0; i < (int)Layer::MAX; i++)
		{
			if(ImGui::MenuItem(enum_to_string((Layer)i).c_str()))
			{
				ChangeLayer((Layer)i);
			}
		}
		ImGui::EndPopup();
	}

	if (ImGui::BeginPopup("Tag##PopUp"))
	{
		for (int i = 0; i < (int)Tag::MAX; i++)
		{
			if (ImGui::MenuItem(enum_to_string((Tag)i).c_str()))
			{
				tag = (Tag)i;
			}
		}
		ImGui::EndPopup();
	}
}

GameObject::GameObject(const GameObject & object)
{
	this->name = object.name;
	this->tag = object.tag;
	this->layer = object.layer;
	this->isActive = object.isActive;
}

GameObject::GameObject(std::string name, Tag tag, Layer layer) :
	name(name), isActive(true), tag(tag), layer(layer)
{
}

GameObject::~GameObject()
{
}

void GameObject::Initialize(std::shared_ptr<GameObject> thisObject)
{
	// 自身の参照を設定
	gameObject = thisObject;
}

void GameObject::Finalize()
{
	for (auto & component : components)
	{
		Component::RemoveComponent(component);
	}
	std::list<std::weak_ptr<Component>>().swap(components);
	std::list<std::weak_ptr<MonoBehaviour>>().swap(behaviours);
}

void GameObject::RemoveComponentList(Component * component)
{
	for (auto itr = components.begin(), end = components.end(); itr != end; ++itr)
	{
		if (itr->lock().get() != component) continue;
		components.erase(itr);
		return;
	}
}

void GameObject::SetTransform(std::weak_ptr<Transform>& transform)
{
	this->transform = transform;
}

void GameObject::SetMonoBehaviour(std::weak_ptr<MonoBehaviour>& behaviour)
{
	behaviours.emplace_back(behaviour);
}

void GameObject::RemoveMonoBehaviour(MonoBehaviour * behaviour)
{
	for (auto itr = behaviours.begin(), end = behaviours.end(); itr != end; ++itr)
	{
		if (itr->lock().get() != behaviour) continue;

		behaviours.erase(itr);
		return;
	}
}

void GameObject::OnCollision(std::weak_ptr<Collision> mine, std::weak_ptr<Collision> other)
{
	for (auto & behaviour : behaviours)
	{
		if (!behaviour.lock()->enable) continue;
		behaviour.lock()->OnCollisionStay(mine, other);
	}
	// 親にも当たったことを知らせる
	auto parent = this->transform.lock()->GetParent();
	if (parent.expired() || parent.lock()->gameObject.expired()) return;

	parent.lock()->gameObject.lock()->OnCollision(mine, other);
}

void GameObject::OnTrigger(std::weak_ptr<Collision> mine, std::weak_ptr<Collision> other)
{
	for (auto & behaviour : behaviours)
	{
		if (!behaviour.lock()->enable) continue;
		behaviour.lock()->OnTriggerStay(mine, other);
	}
	// 親にも当たったことを知らせる
	auto parent = this->transform.lock()->GetParent();
	if (parent.expired() || parent.lock()->gameObject.expired()) return;

	parent.lock()->gameObject.lock()->OnTrigger(mine, other);
}

void GameObject::SetActive(bool active)
{
	isActive = active;

	// 再起関数で自身の子にもセット
	for (auto & child : transform.lock()->GetChilds())
	{
		if (child.lock()->gameObject.expired()) continue;
		child.lock()->gameObject.lock()->SetActive(active);
	}
}

bool GameObject::IsActive()
{
	return isActive;
}

Layer GameObject::ChangeLayer(Layer layer)
{
	this->layer = layer;
	for (auto & com : components)
	{
		com.lock()->ChangeLayer(this->layer);
	}
	return this->layer;
}

void GameObject::SetStop(bool isStop)
{
	this->isStop = isStop;
}

void GameObject::OnDestroy(float limit)
{
	Singleton<GameObjectManager>::Instance()->DestroyGameObject(gameObject, limit);

	for (auto & child : transform.lock()->GetChilds())
	{
		auto & obj = child.lock()->gameObject;
		if (obj.expired()) continue;

		obj.lock()->OnDestroy(limit);
	}
}

void GameObject::LoadSerialized()
{
	// コンポーネントをリストに登録したりする
	for (const std::weak_ptr<Component> & component : components)
	{
		component.lock()->LoadSerialized(component.lock());
	}
}

std::weak_ptr<GameObject> GameObject::SetParent(std::weak_ptr<GameObject> gameObject, SetType type)
{
	transform.lock()->SetParent(gameObject.lock()->transform, type);
	return this->gameObject;
}

std::weak_ptr<GameObject> GameObject::SetChild(std::weak_ptr<GameObject> gameObject, SetType type)
{
	transform.lock()->SetChild(gameObject.lock()->transform, type);
	return this->gameObject;
}

void GameObject::Destroy(std::weak_ptr<GameObject> gameObject, float limit)
{
	if (gameObject.expired()) return;
	gameObject.lock()->OnDestroy(limit);
}
