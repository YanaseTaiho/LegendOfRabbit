#include "Renderer.h"
#include "../../GameObject/GameObject.h"

using namespace FrameWork;

std::list<std::weak_ptr<Renderer>> Renderer::rendererList[(int)Layer::MAX];

void Renderer::LoadSerialized(const std::shared_ptr<Component>& component)
{
	Component::LoadSerialized(component);
	layer = gameObject.lock()->GetLayer();
	AddComponentList(rendererList[(int)layer]);
}

void Renderer::AddComponent()
{
	Component::AddComponent();
	layer = gameObject.lock()->GetLayer();
	AddComponentList(rendererList[(int)layer]);

	Awake();
}

void Renderer::RemoveComponent()
{
	RemoveComponentList(rendererList[(int)layer]);
	Component::RemoveComponent();
}

void Renderer::ChangeLayer(Layer layer)
{
	auto com = RemoveComponentList(rendererList[(int)this->layer]);
	this->layer = layer;
	AddComponentList(rendererList[(int)this->layer], com);
}
