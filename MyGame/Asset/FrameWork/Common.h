#ifndef _FRAMEWORK_COMMON__H__
#define _FRAMEWORK_COMMON__H__

#include "GameObject/GameObject.h"
#include "GameObject/GameObjectManager.h"

#include "Component/Transform/Transform.h"
#include "Component/Animation/Animator.h"
#include "Component/MonoBehaviour/MonoBehaviour.h"
#include "Component/Light/Light.h"
#include "Component/Renderer/Renderer.h"
#include "Component/Rigidbody/Rigidbody.h"
#include "Component/Camera/Camera.h"

#include "Component/Collision/Collision.h"
#include "Component/Collision/Mesh/CollisionMesh.h"
#include "Component/Collision/Sphere/CollisionSphere.h"
#include "Component/Collision/Capsule/CollisionCapsule.h"
#include "Component/Collision/Box/CollisionBox.h"
#include "Component/Collision/Ray/RayCast.h"
#include "Component/Collision/Mesh/MeshCast.h"

#include "Scene/SceneManager.h"
#include "Scene/SceneBase.h"

#include "Animation/AnimationClipManager.h"
#include "Audio/AudioClipManager.h"

#include "Time.h"
#include "FrameTimer.h"
#include "debugPrintf.h"
#include "Rect.h"

#include "Input/Keyboard.h"
#include "Input/Mouse.h"
#include "Input/GamePad.h"

#include "Singleton.h"

#include "FileLoader.h"
#include "MyImGui.h"

using namespace FrameWork;

#endif // !_FRAMEWORK_COMMON__H__

