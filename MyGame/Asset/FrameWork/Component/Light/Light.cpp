#include "Light.h"
#include "../../GameObject/GameObject.h"

using namespace FrameWork;

std::list<std::weak_ptr<Light>> Light::lightList;