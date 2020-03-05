#include "SceneBase.h"
#include "../Common.h"

#include "DirectX/MeshData/LineMesh/DebugLine.h"
#include "DirectX/Common.h"

SceneBase::SceneBase()
{

}

SceneBase::~SceneBase()
{

}

void SceneBase::Initialize()
{

}

void SceneBase::Start()
{
	for (const auto & com : Animator::ComponentList())
	{
		com.lock()->Start();
	}
	for (const auto & com : MonoBehaviour::ComponentList())
	{
		com.lock()->Start();
	}

	isStart = true;
}

void SceneBase::Update()
{
	for (const auto & com : MonoBehaviour::ComponentList())
	{
		if (!com.lock()->IsEnable() || com.lock()->IsStop()) continue;
		com.lock()->Update();
	}

	for (const auto & com : Animator::ComponentList())
	{
		if (!com.lock()->IsEnable() || com.lock()->IsStop()) continue;
		com.lock()->Update();
	}

	for (const auto & com : Rigidbody::ComponentList())
	{
		if (!com.lock()->IsEnable() || com.lock()->IsStop()) continue;
		com.lock()->Update();
	}

	for (int i = 0; i < (int)Layer::MAX; i++)
	{
		for (const auto & col : Collision::CollisionList(i))
		{
			if (!col.lock()->IsEnable() || col.lock()->IsStop()) continue;
			col.lock()->Update();
		}
	}

	Rigidbody::UpdateCollisionJudge();

	for (const auto & com : MonoBehaviour::ComponentList())
	{
		if (!com.lock()->IsEnable() || com.lock()->IsStop()) continue;
		com.lock()->LateUpdate();
	}

	//======= エフェクトの更新 ======//
	Singleton<MyDirectX::EffekseerManager>::Instance()->Update();

	// このタイミングでオブジェクトやコンポーネントの削除処理を行う
	Singleton<GameObjectManager>::Instance()->CleanupDestroyGameObject();
	Component::CleanupDestoryComponent();
}

void SceneBase::Draw()
{
	for (const auto & camera : Camera::ComponentList())
	{
		if (!camera.lock()->IsEnable()) continue;
		camera.lock()->Draw();

		for (const auto & light : Light::ComponentList())
		{
			if (!light.lock()->IsEnable()) continue;
			light.lock()->SetLight();
		}

		// 影の深度テクスチャ作成
		MyDirectX::RendererSystem::SetShadowOption();
		for (int i = 0; i < (int)Layer::MAX; i++)
		{
			const auto & rendererList = Renderer::ComponentList(i);

			for (const auto & renderer : rendererList)
			{
				if (!renderer.lock()->IsEnable()) continue;
				renderer.lock()->DrawShadow();
			}
		}
		// 通常の描画 
		MyDirectX::RendererSystem::SetDefaultOption();
		for (int i = 0; i < (int)Layer::MAX; i++)
		{
			if (i == (int)Layer::Canvas)
				RendererSystem::SetWorldViewProjection2D();

			const auto & rendererList = Renderer::ComponentList(i);

			for (const auto & renderer : rendererList)
			{
				if (!renderer.lock()->IsEnable()) continue;
				//if(editorCamera.lock()->IsVisiblity(renderer.lock()->transform.lock()->GetWorldPosition()))
				renderer.lock()->Draw();
			}
		}

		camera.lock()->Draw();

		//======= エフェクトのレンダリング ======//
		auto dxCamera = std::static_pointer_cast<DXCamera>(camera.lock());
		Rect viewport = dxCamera->viewport;
		float viewWidth = (float)(viewport.right - viewport.left);
		float viewHeight = (float)(viewport.bottom - viewport.top);
		Singleton<EffekseerManager>::Instance()->Draw(dxCamera->transform.lock()->GetWorldMatrix(), viewWidth / viewHeight, dxCamera->nearDistance, dxCamera->farDistance);

		if (isDebug)
		{
			for (const auto & com : MonoBehaviour::ComponentList())
			{
				if (!com.lock()->IsEnable()) continue;
				com.lock()->Draw();
			}
			for (int i = 0; i < (int)Layer::MAX; i++)
			{
				for (const auto & col : Collision::CollisionList(i))
				{
					if (!col.lock()->IsEnable()) continue;
					col.lock()->Draw();
				}
			}

			MyDirectX::DebugLine::DrawRayAll();
		}
	}


	MyDirectX::DebugLine::DrawDataReset();
}

void SceneBase::AddSceneGameObject(std::weak_ptr<GameObject> addObject)
{
	if (!sceneData) return;

	sceneData->gameObjectList.emplace_back(addObject);
}

std::weak_ptr<GameObject> SceneBase::GetPrefabGameObject(std::string name)
{
	return prefab.FindGameObject(name);
}
