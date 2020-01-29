#include "EditorScene.h"
#include "../../Scripts/Common.h"
#include "../../Tool/Menu.h"
#include "../../FrameWork/BindClass.h"
#include "../../Main/manager.h"

// ここで呼ぶことでシリアル化のクラスを登録する
#include <cereal/archives/binary.hpp>

using namespace Input;
using namespace MyDirectX;

static BindClass<Component> componentList;

void EditorScene::CreateComponentList()
{
	// レンダラー
	REGISTER_COMPONENT(componentList, MeshRenderer);
	REGISTER_COMPONENT(componentList, SkinMeshRenderer);
	REGISTER_COMPONENT(componentList, CanvasRenderer);
	REGISTER_COMPONENT(componentList, DXCamera);
	REGISTER_COMPONENT(componentList, Animator);
	REGISTER_COMPONENT(componentList, Rigidbody);
	// ライト
	REGISTER_COMPONENT(componentList, DirectionalLight);
	// コリジョン
	REGISTER_COMPONENT(componentList, CollisionSphere);
	REGISTER_COMPONENT(componentList, CollisionMesh);

	// スクリプト
	REGISTER_COMPONENT(componentList, GameSceneSystem);
	REGISTER_COMPONENT(componentList, PlayerActor);
	REGISTER_COMPONENT(componentList, CameraController);
	REGISTER_COMPONENT(componentList, RotationFixedController);

	// シェーダー
	BindClass<Shader> shaderList;
	REGISTER_COMPONENT(shaderList, MeshShader);
	REGISTER_COMPONENT(shaderList, SkinMeshShader);
	REGISTER_COMPONENT(shaderList, SkyDomeShader);
	REGISTER_COMPONENT(shaderList, ToonMeshShader);
	Singleton<MaterialManager>::Instance()->CreateShaderList(shaderList);

	// アニメションコントローラー
	BindClass<AnimationController> animControllerList;
	REGISTER_COMPONENT(animControllerList, TestAnimationController);
	REGISTER_COMPONENT(animControllerList, RabitAnimationController);
	Animator::CreateAnimationControllerList(animControllerList);
}

EditorScene::EditorScene()
{
	state = State::Stop;
	mode = Mode::Edit;

	CreateComponentList();

	ImGuiStyle& style = ImGui::GetStyle();
	style.ItemSpacing = ImVec2(8.0f, 2.0f);
	style.TouchExtraPadding = ImVec2(0.0f, 2.0f);

	auto & io = ImGui::GetIO();
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	io.ConfigDockingWithShift = false;
	io.ConfigWindowsResizeFromEdges = true;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	//io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleViewports;
	io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleFonts;
	//io.BackendFlags |= ImGuiBackendFlags_PlatformHasViewports;
}

EditorScene::~EditorScene()
{
	Singleton<Tool::Menu>::Release();
	Transform::AddTransform = nullptr;
	Transform::RemoveTransform = nullptr;
}

void EditorScene::Initialize()
{
	Singleton<Tool::Menu>::Create();

	prefab.Load(DATA_FOLDER_PATH);

	CreateEditorCamera();
	CreateManipulator();

	Transform::AddTransform = [&](std::weak_ptr<Transform> add)
	{
		std::shared_ptr<NodeTransform> addNode = std::make_shared<NodeTransform>();
		addNode->transform = add;

		// ペーストされたオブジェクトなら選択する
		for (auto itr = pastObjectNameList.begin(), end = pastObjectNameList.end(); itr != end; ++itr)
		{
			if (*itr == addNode->transform.lock()->gameObject.lock()->name)
			{
				// 現在選択している階層に親子関係を持たせる
				if (!curSelectParentNode.expired())
				{
					addNode->transform.lock()->SetParent(curSelectParentNode.lock()->transform);
				}
				AddSelectableObject(addNode);
				pastObjectNameList.erase(itr);
				break;
			}
		}

		nodeTransformList.emplace_back(addNode);

		isSort = true;
	};

	Transform::RemoveTransform = [&](std::weak_ptr<Transform> remove)
	{
		for (auto itr = selectNodeTransformList.begin(), end = selectNodeTransformList.end(); itr != end; ++itr)
		{
			if (itr->lock()->transform.lock() == remove.lock())
			{
				selectNodeTransformList.erase(itr);
				break;
			}
		}
		for (auto itr = nodeTransformList.begin(), end = nodeTransformList.end(); itr != end; ++itr)
		{
			if ((*itr)->transform.lock() == remove.lock())
			{
				nodeTransformList.erase(itr);
				break;
			}
		}

		isSort = true;	// ソートオン
	};

	auto manager = Singleton<GameObjectManager>::Instance();

	//std::weak_ptr<GameObject> dirLight = manager->CreateGameObject(new GameObject("Light", Tag::None, Layer::Default));
	//dirLight.lock()->AddComponent(new Transform(Vector3(0.0f, 10.0f, 0.0f), Vector3::one(), Quaternion(45.0f, 0.0f, 45.0f)));
	//dirLight.lock()->AddComponent(new DirectionalLight(
	//	Color(1.0f, 1.0f, 1.0f, 1.0f),		// ディフーズ
	//	Color(0.1f, 0.1f, 0.1f, 1.0f)));	// アンビエント

	//{
	//	std::weak_ptr<GameObject> chara2 = manager->CreateGameObject(new GameObject("SuperRabit", Tag::None, Layer::Default));
	//	chara2.lock()->AddComponent(new Transform(Vector3(-30.0f, 0.0f, 0.0f), Vector3(0.08f, 0.08f, 0.08f), Quaternion(0.0f, 0.0f, 0.0f)));
	//	chara2.lock()->AddComponent(new SkinMeshRenderer(FilePathModel("SuperRabit2.fbx"), false));
	//	chara2.lock()->AddComponent(new Animator(new TestAnimationController("SuperRabit2|Idle", 50.0f)));
	//}
	//{
	//	std::weak_ptr<GameObject> chara2 = manager->CreateGameObject(new GameObject("Ellie", Tag::None, Layer::Default));
	//	chara2.lock()->AddComponent(new Transform(Vector3(-30.0f, 100.0f, 30.0f), Vector3(10.0f, 10.0f, 10.0f), Quaternion(0.0f, 0.0f, 0.0f)));
	//	chara2.lock()->AddComponent(new SkinMeshRenderer(FilePathModel("Ellie/Ellie.fbx"), true));
	//	chara2.lock()->AddComponent(new Animator(new TestAnimationController("Ellie|Run", 30.0f)));
	//}
	//{
	//	std::weak_ptr<GameObject> chara2 = manager->CreateGameObject(new GameObject("Baby", Tag::None, Layer::Default));
	//	chara2.lock()->AddComponent(new Transform(Vector3(30.0f, 100.0f, 30.0f), Vector3(10.0f, 10.0f, 10.0f), Quaternion(0.0f, 0.0f, 0.0f)));
	//	chara2.lock()->AddComponent(new SkinMeshRenderer(FilePathModel("Baby2.fbx"), false));
	//	//chara2.lock()->AddComponent(new Animator(new TestAnimationController("Baby2|Catch", 20.0f)));
	//}
	//{
	//	std::weak_ptr<GameObject> chara = manager->CreateGameObject(new GameObject("SmaphoMan", Tag::None, Layer::Default));
	//	chara.lock()->AddComponent(new Transform(Vector3(0.0f, 1.0f, 0.0f), Vector3(3.08f, 3.08f, 3.08f), Quaternion(0.0f, 0.0f, 0.0f)));
	//	chara.lock()->AddComponent(new SkinMeshRenderer(FilePathModel("SmaphoMan.fbx"), true));
	//	chara.lock()->AddComponent(new Animator(new TestAnimationController("SmaphoMan|Deth", 20.0f)));
	//}

	//std::weak_ptr<GameObject> sea = manager->CreateGameObject(new GameObject("sea", Tag::None, Layer::Default));
	//sea.lock()->AddComponent(new Transform(Vector3(0.0f, 0.0f, 0.0f), Vector3(50.0f, 50.0f, 50.0f), Quaternion(0.0f, 0.0f, 0.0f)));
	//sea.lock()->AddComponent(new MeshRenderer(FilePathModel("sea.fbx"), true));
	////sea.lock()->AddComponent(new CollisionMesh(false));

	//{
	//	std::weak_ptr<GameObject> chara = manager->CreateGameObject(new GameObject("Chicken", Tag::None, Layer::Default));
	//	chara.lock()->AddComponent(new Transform(Vector3(0.0f, 10.0f, -50.0f), Vector3(15.0f, 15.0f, 15.0f), Quaternion(0.0f, 0.0f, 0.0f)));
	//	chara.lock()->AddComponent(new MeshRenderer(FilePathModel("Chicken/Chicken.fbx"), true));
	//	/*{
	//		std::weak_ptr<GameObject> chara3 = manager->CreateGameObject(new GameObject("ChickenLitle", Tag::None, Layer::Default));
	//		chara3.lock()->AddComponent(new Transform(Vector3(0.0f, 10.0f, 0.0f), Vector3(5.0f, 5.0f, 5.0f), Quaternion(0.0f, 0.0f, 0.0f)));
	//		chara3.lock()->AddComponent(new MeshRenderer(FilePathModel("Chicken/Chicken.fbx"), true));
	//		chara3.lock()->SetParent(chara);
	//	}*/
	//}
	//
	//{
	//	std::weak_ptr<GameObject> chara = manager->CreateGameObject(new GameObject("SkyDome", Tag::None, Layer::Default));
	//	chara.lock()->AddComponent(new Transform(Vector3(0.0f, 0.0f, 0.0f), Vector3(5000.0f, 5000.0f, 5000.0f), Quaternion(0.0f, 0.0f, 0.0f)));
	//	chara.lock()->AddComponent(new MeshRenderer(FilePathModel("SkyDome1.fbx"), false));
	//}

	///*{
	//	std::weak_ptr<GameObject> chara = manager->CreateGameObject(new GameObject("Plane", Tag::None, Layer::Canvas));
	//	chara.lock()->AddComponent(new Transform(Vector3(0.0f, 0.0f, 0.0f), Vector3(300.0f, 300.0f, 1.0f), Quaternion(0.0f, 0.0f, 0.0f)));
	//	chara.lock()->AddComponent(new CanvasRenderer(FilePathTexture("maple.jpg")));
	//}*/
}

void EditorScene::Start()
{
	if (isStart)
	{
		SceneBase::Start();
	}
}

void EditorScene::Update()
{
	if (mode == Mode::Edit)
	{
		for (auto & com : editorCameraObject->behaviours)
		{
			com.lock()->Update();
			com.lock()->LateUpdate();
		}

		// コリジョンだけ更新する
		for (int i = 0; i < (int)Layer::MAX; i++)
		{
			for (const auto & col : Collision::CollisionList(i))
			{
				if (!col.lock()->IsEnable()) continue;
				col.lock()->Update();
			}
		}
	}

	if (state == State::Play && !isInterruption)
	{
		SceneBase::Update();
	}

	Singleton<GameObjectManager>::Instance()->CleanupDestroyGameObject();
	Component::CleanupDestoryComponent();
}

void EditorScene::Draw()
{
	// フレームレート表示
	int fps = (int)(Time::GetFPS() + 0.1);
	ImGui::Text("%.3f, FPS %1.f", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

	// エディタカメラのビューポート更新
	RECT wRect;
	GetClientRect(RendererSystem::hWnd, &wRect);
	editorCamera.lock()->viewport = Rect(wRect.left, wRect.right, wRect.top, wRect.bottom);

	if (sceneData)
	{
		if (ImGui::Begin("State##Editor"))
		{
			std::weak_ptr<Texture> playButton;
			if (state == State::Stop)
				playButton = Singleton<TextureManager>::Instance()->GetTexture(FilePathTextureEx("Play.png"));
			else
				playButton = Singleton<TextureManager>::Instance()->GetTexture(FilePathTextureEx("Reset.png"));

			if (!playButton.expired())
			{
				if (state == State::Stop)
				{
					if (ImGui::ImageButton((void*)*playButton.lock()->GetShaderResourceView(), ImVec2(30.0f, 30.0f)))
					{
						state = State::Play;

						// 実行直前のデータを保存
						std::shared_ptr<SceneData> saveData = std::shared_ptr<SceneData>(new SceneData());
						saveData->name = sceneData->name;
						// 親がいないオブジェクトだけを選択
						for (auto & obj : sceneData->gameObjectList)
						{
							if (obj.expired())continue;
							if (!obj.lock()->transform.lock()->GetParent().expired()) continue;	// 親がいたら入れない

							saveData->gameObjectList.emplace_back(obj);
						}

						std::stringstream *ss = new std::stringstream();
						cereal::BinaryOutputArchive o_archive(*ss);
						o_archive(saveData);
						editSceneData.reset(ss);

						// ゲーム開始時にStart関数を呼ぶ
						this->isStart = true;
						this->Start();
					}
				}
				else if (state == State::Play)
				{
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
					if (ImGui::ImageButton((void*)*playButton.lock()->GetShaderResourceView(), ImVec2(30.0f, 30.0f)))
					{
						state = State::Stop;
						isInterruption = false;
						this->isStart = false;

						// 実行直前のデータを復元
						Singleton<SceneManager>::Instance()->LoadSceneData(*editSceneData);
						editSceneData.reset();

					}
					ImGui::PopStyleColor();
				}
			}
			std::weak_ptr<Texture> stopButton = Singleton<TextureManager>::Instance()->GetTexture(FilePathTextureEx("Stop.png"));
			if (!stopButton.expired())
			{
				ImGui::SameLine();

				// 中断中
				if (isInterruption)
				{
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
					if (ImGui::ImageButton((void*)*stopButton.lock()->GetShaderResourceView(), ImVec2(30.0f, 30.0f)))
					{
						isInterruption = false;
					}
					ImGui::PopStyleColor();
				}
				// 実行中
				else
				{
					if (ImGui::ImageButton((void*)*stopButton.lock()->GetShaderResourceView(), ImVec2(30.0f, 30.0f)))
					{
						if (state == State::Play) isInterruption = true;
					}
				}
			}
		}
		ImGui::End();
		if (ImGui::Begin("Mode##Editor"))
		{
			if (ImGui::Button("Edit##Editor"))
			{
				mode = Mode::Edit;
			}
			ImGui::SameLine();
			if (ImGui::Button("Game##Editor"))
			{
				mode = Mode::Game;
			}
		}
		ImGui::End();
	}

	if (mode == Mode::Edit)
	{
		DrawScene();
		DrawImGui();
		DrawManipulator();

		DebugLine::DrawRayAll();
		DebugLine::DrawDataReset();
	}
	else if (mode == Mode::Game)
	{
		SceneBase::Draw();
	}

	//DebugLine::DrawLine("Grid", Matrix4::Identity(), Color() * 0.8f);
}

void EditorScene::CreateEditorCamera()
{
	// マネージャーに登録せずここで管理する
	editorCameraObject.reset(new GameObject());
	editorCameraObject->gameObject = editorCameraObject;

	auto editorTransform = std::shared_ptr<Transform>(new Transform(Vector3(0.0f, 50.0f, -50.0f), Vector3::one(), Quaternion(30.0f, 0.0f, 0.0f)));
	editorTransform->gameObject = editorCameraObject;
	editorTransform->transform = editorTransform;
	editorComponentList.push_back(editorTransform);
	
	editorCameraObject->transform = editorTransform;

	auto editorDXCamera = std::shared_ptr<DXCamera>(new DXCamera(0, SCREEN_WIDTH, 0, SCREEN_HEIGHT, 0.5f, 10000.0f));
	editorDXCamera->gameObject = editorCameraObject;
	editorDXCamera->transform = editorTransform;
	editorComponentList.push_back(editorDXCamera);
	editorCamera = editorDXCamera;

	auto editorToolCamera = std::shared_ptr<ToolCamera>(new ToolCamera());
	editorToolCamera->gameObject = editorCameraObject;
	editorToolCamera->transform = editorTransform;
	editorComponentList.push_back(editorToolCamera);
	editorCameraObject->behaviours.push_back(editorToolCamera);
}

void EditorScene::CreateManipulator()
{
	// マネージャーに登録せずここで管理する
	cubeManipulator.reset(new GameObject());
	cubeManipulator->gameObject = cubeManipulator;
	cubeManipulator->AddComponent(new Transform(Vector3(0.0f, 0.0f, 0.0f), Vector3(1.0f, 1.0f, 1.0f), Quaternion(0.0f, 0.0f, 0.0f)));
	cubeManipulator->AddComponent(new MeshRenderer("Asset/Art/Model/Tool/cube.fbx", false, false, false));

	arrowManipulator.reset(new GameObject());
	arrowManipulator->gameObject = arrowManipulator;
	arrowManipulator->AddComponent(new Transform(Vector3(0.0f, 0.0f, 0.0f), Vector3(1.0f, 1.0f, 1.0f), Quaternion(0.0f, 0.0f, 0.0f)));
	arrowManipulator->AddComponent(new MeshRenderer("Asset/Art/Model/Tool/arrow.fbx", false, false, false));
}

void EditorScene::SetAxisColor(AxisType type, const Color & color)
{
	axisColorArray[AxisType::Forward] = Color::blue();
	axisColorArray[AxisType::Right] = Color::red();
	axisColorArray[AxisType::Up] = Color::green();
	axisColorArray[AxisType::All] = Color::white();

	axisColorArray[type] = color;
}

void EditorScene::DrawScene()
{
	editorCamera.lock()->Draw();

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

	editorCamera.lock()->Draw();

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
}

void EditorScene::DrawImGui()
{
	// ここで各マネージャーの処理
	Singleton<Tool::Menu>::Instance()->ImGui();

	ImGuiStyle& style = ImGui::GetStyle();
	style.ItemSpacing = ImVec2(8.0f, 2.0f);
	style.TouchExtraPadding = ImVec2(0.0f, 2.0f);
	DrawHierarchy();

	style.ItemSpacing = ImVec2(8.0f, 4.0f);
	style.TouchExtraPadding = ImVec2(0.0f, 2.0f);

	if (ImGui::Begin("Prefab"))
	{
		const std::string prefablId = "##Prefab";
		float winWidth = ImGui::GetWindowWidth();
		if (ImGui::Button(("Save" + prefablId).c_str(), ImVec2(winWidth * 0.4f, 0)))
		{
			ImGui::OpenPopup("Save?");
		}
		ImGui::SameLine();
		if (ImGui::Button(("Load" + prefablId).c_str(), ImVec2(winWidth * 0.4f, 0)))
		{
			ImGui::OpenPopup("Load?");
		}
		if (ImGui::BeginPopupModal("Save?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("final confirmation.");
			if (ImGui::Button("OK", ImVec2(120, 0)))
			{
				prefab.Save(DATA_FOLDER_PATH);
				ImGui::CloseCurrentPopup();
			}
			ImGui::SetItemDefaultFocus();
			ImGui::SameLine();
			if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
			ImGui::EndPopup();
		}
		if (ImGui::BeginPopupModal("Load?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("final confirmation.");
			if (ImGui::Button("OK", ImVec2(120, 0)))
			{
				prefab.Load(DATA_FOLDER_PATH);
				ImGui::CloseCurrentPopup();
			}
			ImGui::SetItemDefaultFocus();
			ImGui::SameLine();
			if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
			ImGui::EndPopup();
		}

		ImGui::Separator();

		prefab.DrawImGui();
	}
	ImGui::End();

	DrawInspector();

	ImGui::Image((void*)RendererSystem::GetShadowTexture(), ImVec2(200, 200), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1), ImVec4(1, 1, 1, 1));

	// デモ
	ImGui::ShowDemoWindow();
	//ImGui::ShowMetricsWindow();
}

void EditorScene::DrawManipulator()
{
	static bool global = false;

	if (Keyboad::IsTrigger('Q'))
		manipulatorType = Translate;
	else if (Keyboad::IsTrigger('W'))
		manipulatorType = Scale;
	else if (Keyboad::IsTrigger('E'))
		manipulatorType = Rotate;

	if (ImGui::Begin("Manipulator"))
	{
		if (ImGui::Button((global) ? "Global" : "Local"))
			global = !global;

		if (ImGui::RadioButton("Translate", manipulatorType == Translate))
		{
			manipulatorType = Translate;
		}
		ImGui::SameLine();
		if (ImGui::RadioButton("Scale", manipulatorType == Scale))
		{
			manipulatorType = Scale;
		}
		ImGui::SameLine();
		if (ImGui::RadioButton("Rotate", manipulatorType == Rotate))
		{
			manipulatorType = Rotate;
		}
	}
	ImGui::End();

	if (selectNodeTransformList.size() == 0) return;

	Vector2 screenPos;
	Vector3 rayStart, rayEnd;
	POINT point;
	screenPos = Mouse::GetPosition();
	point.x = (int)screenPos.x; point.y = (int)screenPos.y;
	ScreenToClient(RendererSystem::hWnd, &point);
	screenPos.x = (float)point.x; screenPos.y = (float)point.y;

	ImGui::Text("MousePos_screen %3.3f,%3.3f", screenPos.x, screenPos.y);

	editorCamera.lock()->ScreenToWorldPoint(rayStart, screenPos, 0.0f);
	editorCamera.lock()->ScreenToWorldPoint(rayEnd, screenPos, 1.0f);
	Ray ray = Ray(rayStart, rayEnd);

	Vector3 averagePos = Vector3::zero();
	for (auto & t : selectNodeTransformList)
	{
		averagePos += t.lock()->transform.lock()->GetWorldPosition();
	}

	auto trans = selectNodeTransformList.front().lock()->transform;
	Vector3 pos = averagePos / (float)selectNodeTransformList.size();
	Vector3 forward = Vector3::forward();
	Vector3 right = Vector3::right();
	Vector3 up = Vector3::up();
	Quaternion rot = Quaternion::Identity();

	auto renderer = trans.lock()->gameObject.lock()->GetComponent<Renderer>();
	Vector3 modelScale = (renderer.expired()) ? Vector3::one() : renderer.lock()->GetModelScale();
	if (selectNodeTransformList.size() == 1)
	{
		modelScale *= trans.lock()->GetWorldScale();
	}
	else
	{
		float tDist = 0.0f;
		Vector3 tSca = Vector3::one();
		for (auto & t : selectNodeTransformList)
		{
			auto ren = trans.lock()->gameObject.lock()->GetComponent<Renderer>();
			if (!ren.expired() && tSca.LengthSq() < ren.lock()->GetModelScale().LengthSq())
			{
				tSca = ren.lock()->GetModelScale() * t.lock()->transform.lock()->GetWorldScale();
			}

			Vector3 tPos = t.lock()->transform.lock()->GetWorldPosition();
			float tLen = (pos - tPos).Length();
			if (tDist < tLen) tDist = tLen;
		}
		modelScale *= tSca;
	}
	ImGui::Text("ModelScale x,%3.3f y,%3.3f z,%3.3f", modelScale.x, modelScale.y, modelScale.z);

	if (!ImGui::IsAnyWindowHovered())
	{
		if (isMoveToModel)
		{
			if (Mouse::IsPressAny() || Mouse::GetHwheel() != 0.0f)
				isMoveToModel = false;
		}
		else
		{
			if (Keyboad::IsTrigger('F'))
			{
				Vector3 start = editorCamera.lock()->transform.lock()->GetWorldPosition();
				if(std::isnan(start.x)) editorCamera.lock()->transform.lock()->SetLocalPosition(Vector3::zero());
				else if(std::isnan(start.y)) editorCamera.lock()->transform.lock()->SetLocalPosition(Vector3::zero());
				else if(std::isnan(start.z)) editorCamera.lock()->transform.lock()->SetLocalPosition(Vector3::zero());

				isMoveToModel = true;
			}
		}
	}
	if(selectNodeTransformList.size() == 0)
		isMoveToModel = false;

	// モデル追従オン
	if (isMoveToModel)
	{
		Vector3 start = editorCamera.lock()->transform.lock()->GetWorldPosition();
		float len = modelScale.Length() * 0.5f + 50.0f;
		Vector3 dir = editorCamera.lock()->transform.lock()->forward();
		Vector3 end = pos - dir * len;

		editorCamera.lock()->transform.lock()->SetWorldPosition(Vector3::Lerp(start, end, 0.3f));

		if ((start - end).LengthSq() < 0.1f * 0.1f)
			isMoveToModel = false;
	}
	
	if (global && manipulatorType != Scale)
	{
		forward = manipuratorMatrix.forward();
		right = manipuratorMatrix.right();
		up = manipuratorMatrix.up();
		rot = manipuratorMatrix.rotation();
	}
	else if (selectNodeTransformList.size() == 1 || manipulatorType == Scale)
	{
		forward = trans.lock()->forward();
		right = trans.lock()->right();
		up = trans.lock()->up();
		rot = trans.lock()->GetWorldRotation().Normalized();
	}

	float lineLength = 15.0f;
	float lineWidth = 0.8f;
	float maniScale = lineWidth + 0.5f;
	float gizmoSizeClipSpace = 0.01f;
	float lineCollisionWidth = lineWidth * 4.0f;

	Vector3 cameraPos = editorCamera.lock()->transform.lock()->GetWorldPosition();
	Vector3 cameraForward = editorCamera.lock()->transform.lock()->forward();
	Vector3 modelPos = pos;
	Vector3 modelToCamera = cameraPos - modelPos;
	float screenFactor = modelToCamera.Length() * gizmoSizeClipSpace;

	ImGui::Text("screenFactor %3.5f", screenFactor);

	lineLength *= screenFactor;
	lineWidth *= screenFactor;
	maniScale *= screenFactor;
	lineCollisionWidth *= screenFactor;

	if (!isDrag)
	{
		axisColorArray[AxisType::Forward] = Color::blue();
		axisColorArray[AxisType::Right] = Color::red();
		axisColorArray[AxisType::Up] = Color::green();
		axisColorArray[AxisType::All] = Color::white();
	}

	if (isDrag)
	{
		if (Mouse::IsPress(Mouse::Button::Left))
		{
			if (manipulatorType == Translate)
			{
				Vector3 afterPos;
				editorCamera.lock()->ScreenToWorldPoint(afterPos, Mouse::GetPosition(), 0.0f);
				if (Mouse::GetMove() != Vector2::zero())
				{
					float cameraDot = Mathf::Absf(Vector3::Dot(modelToCamera.Normalized(), axisDir));
					float cameraFcator = 1.0f + ((cameraDot < 0.0f) ? 0.0f : cameraDot);
					float dirFactor = screenFactor;
					if (axisType != AxisType::All)
						dirFactor *= Vector3::Dot((afterPos - beforePos), axisDir);
					else
						axisDir = (afterPos - beforePos);

					Vector3 beforePosistion = manipuratorMatrix.position();
					Vector3 position = manipuratorMatrix.position();
					position += axisDir * dirFactor * cameraFcator * 150.0f;

					for (auto & child : manipuratorChilds)
					{
						if (child.expired()) continue;
						Matrix4 localMat;

						if (global)
						{
							manipuratorMatrix.SetMatrix(beforePosistion, Vector3::one(), Quaternion::Identity());
						}
						else
						{
							manipuratorMatrix.SetMatrix(beforePosistion, Vector3::one(), child.lock()->GetWorldRotation().Normalized());
						}
						// 親子関係セット
						localMat = manipuratorMatrix.Inverse() * child.lock()->worldMatrix;
						child.lock()->SetLocalMatrix(localMat);

						manipuratorMatrix.SetMatrix(position, Vector3::one(), manipuratorMatrix.rotation());
						// 親子関係リセット
						child.lock()->worldMatrix = manipuratorMatrix * child.lock()->localMatrix;
						child.lock()->SetLocalMatrix(Matrix4(child.lock()->GetWorldPosition(), child.lock()->GetLocalScale(), child.lock()->GetWorldRotation().Normalized()));
						child.lock()->UpdateChilds();
					}
				}
				beforePos = afterPos;
			}
			else if (manipulatorType == Scale)
			{
				Vector3 afterPos;
				editorCamera.lock()->ScreenToWorldPoint(afterPos, Mouse::GetPosition(), 0.0f);
				Vector2 mouseMove = Mouse::GetMove();
				if (mouseMove != Vector2::zero())
				{
					for (auto & child : manipuratorChilds)
					{
						if (child.expired()) continue;
						Vector3 modSca;
						float dirFactor = 1.0f;
						float scaleFactor = 0.8f;

						if (global)
						{
							modSca = Vector3::one();
							manipuratorMatrix.SetMatrix(manipuratorMatrix.position(), modSca, manipuratorMatrix.rotation().Normalized());
						}
						else
						{
							modSca = Vector3::one();
							manipuratorMatrix.SetMatrix(child.lock()->GetWorldPosition(), modSca, child.lock()->GetWorldRotation().Normalized());
						}

						// 親子関係セット
						Matrix4 localMat = manipuratorMatrix.Inverse() * child.lock()->worldMatrix;
						child.lock()->SetLocalMatrix(localMat);

						/*if (axisType == AxisType::Forward)
							axisDir = manipuratorMatrix.forward();
						if (axisType == AxisType::Right)
							axisDir = manipuratorMatrix.right();
						if (axisType == AxisType::Up)
							axisDir = manipuratorMatrix.up();*/
						if (axisType == AxisType::Forward)
							axisDir = child.lock()->forward();
						if (axisType == AxisType::Right)
							axisDir = child.lock()->right();
						if (axisType == AxisType::Up)
							axisDir = child.lock()->up();

						// ここでスケール変換
						if (axisType != AxisType::All)
							dirFactor *= Vector3::Dot((afterPos - beforePos), axisDir);
						else
						{
							dirFactor *= (mouseMove.x - mouseMove.y) * 0.001f;
							axisDir = Vector3::one();
						}

						float cameraDot = Mathf::Absf(Vector3::Dot(modelToCamera.Normalized(), axisDir));
						float cameraFcator = 1.0f + ((cameraDot < 0.0f) ? 0.0f : cameraDot);

						modSca = child.lock()->transform.lock()->GetLocalScale();

						if (axisType == AxisType::Forward)
							modSca.z += modSca.z * dirFactor * cameraFcator * scaleFactor;
						if (axisType == AxisType::Right)
							modSca.x += modSca.x * dirFactor * cameraFcator * scaleFactor;
						if (axisType == AxisType::Up)
							modSca.y += modSca.y * dirFactor * cameraFcator * scaleFactor;
						if (axisType == AxisType::All)
							modSca += modSca * dirFactor * cameraFcator * scaleFactor;
						/*else
						{
							modSca += axisDir * dirFactor * cameraFcator * scaleFactor;
						}*/

						child.lock()->SetLocalMatrix(Matrix4(child.lock()->GetLocalPosition(), modSca, child.lock()->GetLocalRotation()));
						/*child.lock()->worldMatrix = child.lock()->localMatrix;
						child.lock()->UpdateChilds();*/
					
						//manipuratorMatrix.SetMatrix(manipuratorMatrix.position(), modSca, manipuratorMatrix.rotation());

						 //親子関係リセット
						child.lock()->worldMatrix = manipuratorMatrix * child.lock()->localMatrix;
						child.lock()->SetLocalMatrix(Matrix4(child.lock()->GetWorldPosition(), child.lock()->GetLocalScale(), child.lock()->GetWorldRotation().Normalized()));
						child.lock()->UpdateChilds();
					}
				}
				beforePos = afterPos;
			}
			else if (manipulatorType == Rotate)
			{
				Vector3 afterPos;
				editorCamera.lock()->ScreenToWorldPoint(afterPos, Mouse::GetPosition(), 0.0f);
				Vector2 mouseMove = Mouse::GetMove();
				if (mouseMove != Vector2::zero())
				{
					float cameraDot = Mathf::Absf(Vector3::Dot(modelToCamera.Normalized(), axisDir));
					float cameraFcator = 1.0f + ((cameraDot < 0.0f) ? 0.0f : cameraDot);
					float dirFactor = Vector3::Dot(afterPos - beforePos, axisDir);
					float rotAngle = dirFactor * cameraFcator * 100.0f;

					Vector3 axisAngle;
					if (axisType == AxisType::Forward)
						axisAngle = forward;
					if (axisType == AxisType::Right)
						axisAngle = right;
					if (axisType == AxisType::Up)
						axisAngle = up;
					if (axisType == AxisType::All)
					{
						axisAngle = modelToCamera;
						rotAngle *= 0.01f;
					}
					Quaternion beforeRotation = manipuratorMatrix.rotation();
					Quaternion q = Quaternion::AxisAngle(axisAngle, rotAngle).Normalize();

					for (auto & child : manipuratorChilds)
					{
						if (child.expired()) continue;
						if (global)
						{
							manipuratorMatrix.SetMatrix(manipuratorMatrix.position(), Vector3::one(), beforeRotation);
						}
						else
						{
							manipuratorMatrix.SetMatrix(child.lock()->GetWorldPosition(), Vector3::one(), beforeRotation);
						}
						// 親子関係セット
						Matrix4 localMat = manipuratorMatrix.Inverse() * child.lock()->worldMatrix;
						child.lock()->SetLocalMatrix(localMat);

						manipuratorMatrix.SetMatrix(manipuratorMatrix.position(), Vector3::one(), q * manipuratorMatrix.rotation());

						// 親子関係リセット
						child.lock()->worldMatrix = manipuratorMatrix * child.lock()->localMatrix;
						child.lock()->SetLocalMatrix(Matrix4(child.lock()->GetWorldPosition(), child.lock()->GetLocalScale(), child.lock()->GetWorldRotation().Normalized()));
						child.lock()->UpdateChilds();
					}
				}
				beforePos = afterPos;
			}
		}
		else
		{
			isDrag = false;
			// マニピュレーターを離した時
			for (auto & child : manipuratorChilds)
			{
				if (child.expired()) continue;

				// 親になるトランスフォームの逆行列を掛ける
				Matrix4 localMat;
				localMat = child.lock()->worldMatrix;

				if (!child.lock()->parent.expired())
				{
					// 親になるトランスフォームの逆行列を掛ける
					localMat = child.lock()->parent.lock()->GetWorldMatrix().Inverse() * child.lock()->worldMatrix;
					localMat.SetMatrix(localMat.position(), child.lock()->GetLocalScale() / child.lock()->parent.lock()->GetWorldScale(), localMat.rotation());
				}
				child.lock()->SetLocalMatrix(localMat);
			}
			std::vector<std::weak_ptr<Transform>>().swap(manipuratorChilds);
			manipuratorMatrix = Matrix4::Identity();
		}
	}

	RendererSystem::SetDepthEnable(false);

	bool isBeforeDrag = isDrag;

	// 当たり判定
	if (manipulatorType == Translate || manipulatorType == Scale)
	{
		float meshScale = maniScale;
		if (manipulatorType == Scale)
			meshScale *= 0.7f;

		axisMatrixArray[AxisType::Forward] = Matrix4(pos + forward * lineLength, Vector3::one() * meshScale, rot);
		axisMatrixArray[AxisType::Right] = Matrix4(pos + right * lineLength, Vector3::one() * meshScale, Quaternion::AxisAngle(up, 90.0f) * rot);
		axisMatrixArray[AxisType::Up] = Matrix4(pos + up * lineLength, Vector3::one() * meshScale, Quaternion::AxisAngle(right, -90.0f) *  rot);
		axisMatrixArray[AxisType::All] = Matrix4(pos, Vector3::one() * maniScale, rot);

		if (!isDrag)
		{
			RayCastInfo castInfo;
			float hitDistance = 0.0f;
			if (RayCast::JudgeCapsule(ray, pos, pos + forward * lineLength, lineCollisionWidth, &castInfo))
			{
				SetAxisColor(AxisType::Forward, Color::orange());
				hitDistance = castInfo.distance;
			}
			if (RayCast::JudgeCapsule(ray, pos, pos + right * lineLength, lineCollisionWidth, &castInfo))
			{
				if (hitDistance == 0.0f || hitDistance > castInfo.distance)
				{
					SetAxisColor(AxisType::Right, Color::orange());
					hitDistance = castInfo.distance;
				}
			}
			if (RayCast::JudgeCapsule(ray, pos, pos + up * lineLength, lineCollisionWidth, &castInfo))
			{
				if (hitDistance == 0.0f || hitDistance > castInfo.distance)
				{
					SetAxisColor(AxisType::Up, Color::orange());
					hitDistance = castInfo.distance;
				}
			}
			if (RayCast::JudgeSphere(ray, pos, maniScale * 2.0f, &castInfo))
			{
				SetAxisColor(AxisType::All, Color::orange());
			}

			if (Mouse::IsTrigger(Mouse::Button::Left))
			{
				static auto InputManipulator = [&](AxisType type, const Vector3 & axis)
				{
					if (axisColorArray[type] == Color::orange())
					{
						isDrag = true;
						axisType = type;
						axisDir = axis;
						editorCamera.lock()->ScreenToWorldPoint(beforePos, Mouse::GetPosition(), 0.0f);
					
						beforeSca = trans.lock()->GetLocalScale();
					}
				};
				InputManipulator(AxisType::Forward, forward);
				InputManipulator(AxisType::Right, right);
				InputManipulator(AxisType::Up, up);
				InputManipulator(AxisType::All, editorCamera.lock()->transform.lock()->right());
			}
		}	// !isDrag
	}
	else if (manipulatorType == Rotate)
	{
		RayCastInfo castInfo;
		Vector3 colCircle;
		float hitDistance = 0.0f;
		float circleScale = lineLength * 0.9f;
		float whiteCircleScale = circleScale * 1.2f;
		Vector3 posDir = modelToCamera.Normalized();

		static auto JudgeCilinder = [&](const Vector3 & axis, AxisType type)
		{
			colCircle = axis * lineCollisionWidth * 0.5f;
			if (RayCast::JudgeCilinder(ray, pos - colCircle, pos + colCircle, circleScale, &castInfo))
			{
				Vector3 hitPoint = (castInfo.point - pos);
				hitPoint -= Vector3::Dot(hitPoint, colCircle.Normalize()) * colCircle;
				float hitRange = hitPoint.Length();

				Vector3 angleDir = posDir - axis * (Vector3::Dot(axis, posDir)); angleDir.Normalize();
				if (circleScale - lineCollisionWidth * 0.5f < hitRange && Vector3::Dot(hitPoint, angleDir) >= 0)
				{
					SetAxisColor(type, Color::orange());
					hitDistance = castInfo.distance;
				}
			}
		};

		if (!isDrag)
		{
			JudgeCilinder(forward, AxisType::Forward);
			JudgeCilinder(right, AxisType::Right);
			JudgeCilinder(up, AxisType::Up);
			colCircle = posDir * lineCollisionWidth * 0.5f;
			if (RayCast::JudgeCilinder(ray, pos - colCircle, pos + colCircle, whiteCircleScale, &castInfo))
			{
				Vector3 hitPoint = (castInfo.point - pos);
				hitPoint -= Vector3::Dot(hitPoint, colCircle.Normalize()) * colCircle;
				float hitRange = hitPoint.Length();

				if (whiteCircleScale - lineCollisionWidth * 0.5f < hitRange)
				{
					if (hitDistance == 0.0f || hitDistance > castInfo.distance)
					{
						SetAxisColor(AxisType::All, Color::orange());
						hitDistance = castInfo.distance;
					}
				}
			}

			if (Mouse::IsTrigger(Mouse::Button::Left))
			{
				static auto InputManipulator = [&](AxisType type, const Vector3 & axis)
				{
					if (axisColorArray[type] == Color::orange())
					{
						isDrag = true;
						axisType = type;
						axisDir = axis;
						editorCamera.lock()->ScreenToWorldPoint(beforePos, Mouse::GetPosition(), 0.0f);
						beforeSca = trans.lock()->GetLocalScale();
					}
				};
				InputManipulator(AxisType::Forward, Vector3::Cross(forward, modelToCamera).Normalized());
				InputManipulator(AxisType::Right, Vector3::Cross(right, modelToCamera).Normalized());
				InputManipulator(AxisType::Up, Vector3::Cross(up, modelToCamera).Normalized());
				InputManipulator(AxisType::All, editorCamera.lock()->transform.lock()->right());
			}
		}	// !isDrag

		Quaternion angle, axisRot;
		Vector3 maniDir;

		// 前方
		axisRot = rot;
		maniDir = posDir - forward * (Vector3::Dot(forward, posDir)); maniDir.Normalize();
		angle = Quaternion::FromToRotation(up, maniDir) * axisRot;
		axisMatrixArray[AxisType::Forward] = Matrix4(pos, Vector3::one() * circleScale, angle.Normalized());
		// 右方向
		axisRot = Quaternion::AxisAngle(up, -90.0f) * rot;
		maniDir = posDir - right * (Vector3::Dot(right, posDir)); maniDir.Normalize();
		angle = Quaternion::FromToRotation(up, maniDir) * axisRot;
		axisMatrixArray[AxisType::Right] = Matrix4(pos, Vector3::one() * circleScale, angle.Normalized());
		// 上方向
		axisRot = Quaternion::AxisAngle(right, 90.0f) * rot;
		maniDir = posDir - up * (Vector3::Dot(up, posDir)); maniDir.Normalize();
		angle = Quaternion::FromToRotation(forward, maniDir) * axisRot;
		axisMatrixArray[AxisType::Up] = Matrix4(pos, Vector3::one() * circleScale, angle.Normalized());
		// カメラ方向
		axisMatrixArray[AxisType::All] = Matrix4(pos, Vector3::one() * whiteCircleScale, Quaternion::LookRotation(posDir));
	}

	// マニピュレーターを掴んで変更する直前
	if (isDrag && !isBeforeDrag)
	{
		manipuratorMatrix.SetMatrix(pos, Vector3::one(), rot);
		for (auto & node : selectNodeTransformList)
		{
			bool isHit = false;
			for (auto & par : selectNodeTransformList)
			{
				if (par.lock() != node.lock() && IsChildNodeTransform(par, node)) {
					isHit = true;
					break;
				}
			}
			if (isHit) continue;

			auto & child = node.lock()->transform;

			manipuratorChilds.emplace_back(child);
		}
	}


	// 描画
	static auto ManipulatorDraw = [](std::shared_ptr<GameObject>& object, const Matrix4 & matrix, const Color& color)
	{
		object->transform.lock()->SetWorldMatrix(matrix);
		Material & material = object->GetComponent<MeshRenderer>().lock()->model.lock()->meshNode[0].subsetArray[0].material;
		material.materialParam.diffuse = color;
		object->GetComponent<MeshRenderer>().lock()->Draw();
	};

	if (manipulatorType == Translate)
	{
		DebugLine::DrawRayMoment(pos, forward, lineLength, axisColorArray[AxisType::Forward], lineWidth);
		ManipulatorDraw(arrowManipulator, axisMatrixArray[AxisType::Forward], axisColorArray[AxisType::Forward]);
		DebugLine::DrawRayMoment(pos, right, lineLength, axisColorArray[AxisType::Right], lineWidth);
		ManipulatorDraw(arrowManipulator, axisMatrixArray[AxisType::Right], axisColorArray[AxisType::Right]);
		DebugLine::DrawRayMoment(pos, up, lineLength, axisColorArray[AxisType::Up], lineWidth);
		ManipulatorDraw(arrowManipulator, axisMatrixArray[AxisType::Up], axisColorArray[AxisType::Up]);

		ManipulatorDraw(cubeManipulator, axisMatrixArray[AxisType::All], axisColorArray[AxisType::All]);
	}
	else if (manipulatorType == Scale)
	{
		DebugLine::DrawRayMoment(pos, forward, lineLength, axisColorArray[AxisType::Forward], lineWidth);
		ManipulatorDraw(cubeManipulator, axisMatrixArray[AxisType::Forward], axisColorArray[AxisType::Forward]);
		DebugLine::DrawRayMoment(pos, right, lineLength, axisColorArray[AxisType::Right], lineWidth);
		ManipulatorDraw(cubeManipulator, axisMatrixArray[AxisType::Right], axisColorArray[AxisType::Right]);
		DebugLine::DrawRayMoment(pos, up, lineLength, axisColorArray[AxisType::Up], lineWidth);
		ManipulatorDraw(cubeManipulator, axisMatrixArray[AxisType::Up], axisColorArray[AxisType::Up]);

		ManipulatorDraw(cubeManipulator, axisMatrixArray[AxisType::All], axisColorArray[AxisType::All]);
	}
	else if (manipulatorType == Rotate)
	{
		float circleWidth = lineWidth * 0.8f;

		DebugLine::DrawLine("Circle", Matrix4(pos, axisMatrixArray[AxisType::Forward].scale(), axisMatrixArray[AxisType::All].rotation()), Color::black() * 0.4f);
		DebugLine::DrawLine("Circle", axisMatrixArray[AxisType::All], axisColorArray[AxisType::All], circleWidth);
		DebugLine::DrawLine("HalfCircle", axisMatrixArray[AxisType::Forward], axisColorArray[AxisType::Forward], circleWidth);
		DebugLine::DrawLine("HalfCircle", axisMatrixArray[AxisType::Right], axisColorArray[AxisType::Right], circleWidth);
		DebugLine::DrawLine("HalfCircle", axisMatrixArray[AxisType::Up], axisColorArray[AxisType::Up], circleWidth);

	}

	RendererSystem::SetDepthEnable(true);
}

void EditorScene::DrawHierarchy()
{
	int cnt = 0;
	const char* popupName = "Popup##Hierarchy";

	// ノードのソート
	if (isSort)
	{
		SortNodeTransform();
		isSort = false;
	}

	if (Mouse::IsTrigger(Mouse::Button::Right))
		selectPopupObject.reset();

	// 選択オブジェクトを特定の条件で解除
	if (Mouse::IsRelease(Mouse::Button::Left))
	{
		if (!Keyboad::IsPress(VK_LCONTROL) && !Keyboad::IsPress(VK_LSHIFT))
		{
			if (isToutchHierarchyObject && !isDragDrop)
				ClearSelectableObject();
		}
		isToutchHierarchyObject = false;
		isDragDrop = false;
	}

	if (ImGui::Begin("Hierarchy"))
	{
		if (!sceneData)
		{
			ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(1.0f, 0.2f, 0.2f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(1.0f, 0.4f, 0.4f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
			ImGui::CollapsingHeader("Not Scene", ImGuiTreeNodeFlags_Selected);
			ImGui::PopStyleColor(4);
		}
		else
		{
			ImVec2 winSize = ImGui::GetWindowSize();
			winSize.x -= ImGui::GetCursorPosX();
			winSize.y -= ImGui::GetCursorPosY();
			if (ImGui::BeginChild("HierarchyChild", winSize))
			{
				if (ImGui::IsWindowHovered() && Mouse::IsTrigger(Mouse::Button::Left))
				{
					isToutchHierarchyObject = true;
				}

				if (ImGui::BeginChild("Hierarchy##SceneButton", ImVec2(0.0f, 30.0f)))
				{
					// ショートカットキー
					if (Keyboad::IsPress(VK_LCONTROL) && Keyboad::IsTrigger('S'))
						ImGui::OpenPopup("SaveScene?");

					float winWidth = ImGui::GetWindowWidth();
					if (ImGui::Button(("SaveScene" + std::string("##Popup")).c_str(), ImVec2(winWidth, 0.0f)))
					{
						ImGui::OpenPopup("SaveScene?");
					}

					ImGui::Separator();

					if (ImGui::BeginPopupModal("SaveScene?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
					{
						ImGui::Text("final confirmation.");
						if (ImGui::Button("OK", ImVec2(120, 0)))
						{
							Singleton<SkinMeshManager>::Instance()->SaveMesh(DATA_FOLDER_PATH);
							Singleton<MeshManager>::Instance()->SaveMesh(DATA_FOLDER_PATH);
							Singleton<MaterialManager>::Instance()->SaveMaterial(DATA_FOLDER_PATH);
							Singleton<TextureManager>::Instance()->SaveTexture(DATA_FOLDER_PATH);
							prefab.Save(DATA_FOLDER_PATH);
							Singleton<SceneManager>::Instance()->SaveSceneData();
							ImGui::CloseCurrentPopup();
						}
						ImGui::SetItemDefaultFocus();
						ImGui::SameLine();
						if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
						ImGui::EndPopup();
					}
				}
				ImGui::EndChild();

				if (ImGui::CollapsingHeader((sceneData->name + "##SceneData").c_str(), ImGuiTreeNodeFlags_DefaultOpen))
				{
					ImGui::Indent();
					if (ImGui::IsWindowHovered())
					{
						if (Mouse::IsTrigger(Mouse::Button::Right))
						{
							selectPopupObject.reset();
							ImGui::OpenPopup(popupName);
						}
					}

					auto beforeSelectPopup = selectPopupObject;

					for (auto & node : nodeTransformList)
					{
						if (!node->nodeParent.expired()) continue;

						std::string strId = std::string("##") + std::to_string(cnt);
						bool beforeOpen = node->isTreeNodeOpen;

						// 並び替え用の隙間
						ImGui::InvisibleButton("##dammy", ImVec2(ImGui::GetWindowSize().x, 0.5f));
						// ここに入ると並び替え
						if (ImGui::BeginDragDropTarget())
						{
							if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(NodeTransform::NODE_TRANSFORM_LIST))
							{
								auto & payload_n = *(std::list<std::weak_ptr<NodeTransform>>*)payload->Data;
								std::list<std::shared_ptr<NodeTransform>>::iterator parentItr;
								GetItrNodeTransform(node, parentItr);
								auto childItr = parentItr;

								// 階層構造になるように並び替えをする
								for (auto & transNode : payload_n)
								{
									// 自身のイテレーターなら何もせず次へ
									if (childItr != nodeTransformList.end() && *childItr == transNode.lock())
									{
										continue;
									}
									bool isHit = false;
									for (auto & transPay : payload_n)
									{
										if (transPay.lock() == transNode.lock()) continue;
										if (IsChildNodeTransform(transPay, transNode))
										{
											isHit = true;
											break;
										}
									}
									if (isHit) continue;

									transNode.lock()->transform.lock()->ReleaseParent();

									std::shared_ptr<NodeTransform> insert;
									EraseNodeTransform(transNode, insert);

									if (childItr != nodeTransformList.end())
									{
										childItr = nodeTransformList.insert(childItr, insert);
										++childItr;
									}
									else
									{
										nodeTransformList.emplace_back(insert);
									}
									// 自身の子階層も同じように整列させる
									for (auto & child : transNode.lock()->nodeChilds)
									{
										InsertChildNodeTransform(child.lock(), childItr);
									}
								}

								isSort = true;	// ソートオン
							}
							ImGui::EndDragDropTarget();
						}

						bool isTreeOpenFrame = false;
						if (node->nodeChilds.size() > 0)
						{
							ImGui::SetNextTreeNodeOpen(beforeOpen);
							node->isTreeNodeOpen = ImGui::TreeNodeEx((std::string("##NodeTransform") + std::to_string(cnt)).c_str(), ImGuiWindowFlags_ChildMenu);
							if (beforeOpen != node->isTreeNodeOpen)
								isTreeOpenFrame = true;
							ImGui::SameLine();
						}
						else
						{
							node->isTreeNodeOpen = false;
						}

						DrawGameObject(node, strId, popupName);

						// 階層が変化したフレームはノードをいじらない
						if ((isTreeOpenFrame || beforeOpen == node->isTreeNodeOpen)
							&& node->isTreeNodeOpen)
						{
							cnt++;
							for (auto & child : node->nodeChilds)
							{
								DrawNodeChilid(child, cnt, popupName);
								cnt++;
							}
							ImGui::TreePop();
						}
						cnt++;
					}

					// オブジェクトコピー関数
					auto CopyObject = [&]()
					{
						copyDataList.clear();
						if (selectNodeTransformList.size() > 0)
						{
							for (auto & select : selectNodeTransformList)
							{
								// 選択オブジェクトに同じのがいればはじく
								bool isHit = false;
								for (auto & transSelect : selectNodeTransformList)
								{
									if (transSelect.lock() == select.lock()) continue;
									if (IsChildNodeTransform(transSelect, select))
									{
										isHit = true;
										break;
									}
								}
								if (isHit) continue;

								// シリアル化
								std::shared_ptr<GameObject> copyObject = select.lock()->transform.lock()->gameObject.lock();
								std::stringstream *ss = new std::stringstream();
								{
									cereal::BinaryOutputArchive o_archive(*ss);
									o_archive(copyObject);
								}
								copyDataList.emplace_back(ss);
							}
						}
						else if (!selectPopupObject.expired())
						{
							// シリアル化
							std::shared_ptr<GameObject> copyObject = selectPopupObject.lock()->transform.lock()->gameObject.lock();
							std::stringstream *ss = new std::stringstream();
							{
								cereal::BinaryOutputArchive o_archive(*ss);
								o_archive(copyObject);
							}
							copyDataList.emplace_back(ss);
							selectPopupObject.reset();
						}
					};

					// オブジェクトペースト関数
					auto PasteObject = [&]()
					{
						for (auto & copyData : copyDataList)
						{
							if (selectNodeTransformList.size() > 0)
							{
								// 選択しているリストの先頭のデータを取り出す
								auto & frontNode = selectNodeTransformList.front();
								curSelectParentNode = frontNode.lock()->nodeParent;
								frontNode.lock()->isSelect = false;
								selectNodeTransformList.pop_front();
							}
							// これから生成するオブジェクト名を追加

							// シリアル化したものを新しくインスタンス生成
							std::shared_ptr<GameObject> copyObj;
							{
								cereal::BinaryInputArchive i_archive(*copyData);
								i_archive(copyObj);

								pastObjectNameList.emplace_back(copyObj->name);
								Singleton<GameObjectManager>::Instance()->Instantiate(copyObj);
							}
							// ストリームの位置を初期化
							copyData->seekg(0);
						}
					};
					// オブジェクト削除関数
					auto DeleteObject = [&]()
					{
						if (selectNodeTransformList.size() > 0)
						{
							for (auto & select : selectNodeTransformList)
							{
								GameObject::Destroy(select.lock()->transform.lock()->gameObject);
							}
							selectNodeTransformList.clear();
						}
						else if (!selectPopupObject.expired())
						{
							GameObject::Destroy(selectPopupObject.lock()->transform.lock()->gameObject);
							selectPopupObject.reset();
						}
					};

					// ショートカット
					if (ImGui::IsWindowHovered() || !ImGui::IsAnyWindowHovered())
					{
						if (Keyboad::IsPress(VK_LCONTROL) && Keyboad::IsTrigger('C'))
							CopyObject();
						if (Keyboad::IsPress(VK_LCONTROL) && Keyboad::IsTrigger('V'))
							PasteObject();
						if (Keyboad::IsTrigger(VK_DELETE))
							DeleteObject();

						// 選択中のオブジェクトの子階層のノードを全て開く
						if (Keyboad::IsPress(VK_LCONTROL) && Keyboad::IsTrigger('O'))
						{
							for (auto & select : selectNodeTransformList)
							{
								OpneTreeNodeTransformChilds(select);
							}
						}
					}

					if (beforeSelectPopup.lock() != selectPopupObject.lock())
						ImGui::OpenPopup(popupName);

					if (ImGui::BeginPopup(popupName))
					{
						if (ImGui::BeginMenu("Create##Hierarchy"))
						{
							if (ImGui::MenuItem("Empty GameObject"))
							{
								auto object = Singleton<GameObjectManager>::Instance()->CreateGameObject(new GameObject("NewObject", Tag::None, Layer::Default));
								object.lock()->AddComponent(new Transform());
							}
							if (ImGui::MenuItem("Mesh GameObject"))
							{
								auto object = Singleton<GameObjectManager>::Instance()->CreateGameObject(new GameObject("NewObject", Tag::None, Layer::Default));
								object.lock()->AddComponent(new Transform());
								object.lock()->AddComponent(new MeshRenderer());
							}
							if (ImGui::MenuItem("SkinMesh GameObject"))
							{
								auto object = Singleton<GameObjectManager>::Instance()->CreateGameObject(new GameObject("NewObject", Tag::None, Layer::Default));
								object.lock()->AddComponent(new Transform());
								object.lock()->AddComponent(new SkinMeshRenderer());
								object.lock()->AddComponent(new Animator());
							}
							ImGui::EndMenu();
						}
						if (!selectPopupObject.expired() || selectNodeTransformList.size() > 0)
						{
							if (ImGui::MenuItem("Copy##Hierarchy"))
							{
								CopyObject();
							}
						}
						if (copyDataList.size() > 0)
						{
							if (ImGui::MenuItem("Paste##Hierarchy"))
							{
								PasteObject();
							}
						}
						if (!selectPopupObject.expired() || selectNodeTransformList.size() > 0)
						{
							if (ImGui::MenuItem("Delete##Hierarchy"))
							{
								DeleteObject();
							}
						}
						ImGui::EndPopup();
					}

					//ImGui::TreePop();
				}	// !ImGui::CollapsingHeader()

			} ImGui::EndChild();// !HierarchyChild

			// プレハブのゲームオブジェクト生成
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(prefab.GAMEOBJECT))
				{
					auto & payload_n = *(std::shared_ptr<GameObject>*)payload->Data;

					Singleton<GameObjectManager>::Instance()->Instantiate(payload_n);
				}
			}


		} // !sceneData
	}

	ImGui::End();
}

void EditorScene::DrawNodeChilid(std::weak_ptr<NodeTransform> parent, int& cnt, const char* popupName)
{
	std::string strId = std::string("##") + std::to_string(cnt);
	bool beforeOpen = parent.lock()->isTreeNodeOpen;

	// 並び替え用の隙間
	ImGui::InvisibleButton("##dammy", ImVec2(ImGui::GetWindowSize().x, 0.5f));
	// ここに入ると並び替え
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(NodeTransform::NODE_TRANSFORM_LIST))
		{
			auto & payload_n = *(std::list<std::weak_ptr<NodeTransform>>*)payload->Data;
			std::list<std::shared_ptr<NodeTransform>>::iterator parentItr;
			GetItrNodeTransform(parent, parentItr);
			auto childItr = parentItr;

			// 階層構造になるように並び替えをする
			for (auto & transNode : payload_n)
			{
				// これから子になるオブジェクトが既に親子関係なら何もしない
				if (IsChildNodeTransform(transNode, parent))
					continue;

				// 自身のイテレーターなら何もせず次へ
				if (childItr != nodeTransformList.end() && *childItr == transNode.lock())
				{
					continue;
				}
				bool isHit = false;
				for (auto & transPay : payload_n)
				{
					if (transPay.lock() == transNode.lock()) continue;
					if (IsChildNodeTransform(transPay, transNode))
					{
						isHit = true;
						break;
					}
				}
				if (isHit) continue;
				// 一旦親を切る
				transNode.lock()->transform.lock()->ReleaseParent();

				int num = 0;
				std::weak_ptr<Transform> & parentTranform = parent.lock()->nodeParent.lock()->transform;
				for (auto & c : parentTranform.lock()->GetChilds())
				{
					if (c.lock() == parent.lock()->transform.lock())
						break;
					num++;
				}
				// 目的の番号に挿入
				if (Keyboad::IsPress('L'))
				{
					transNode.lock()->transform.lock()->SetParent(parentTranform, SetType::Local, num);
				}
				else
				{
					transNode.lock()->transform.lock()->SetParent(parentTranform, SetType::World, num);
				}

				std::shared_ptr<NodeTransform> insert;
				EraseNodeTransform(transNode, insert);

				if (childItr != nodeTransformList.end())
				{
					childItr = nodeTransformList.insert(childItr, insert);
					++childItr;
				}
				else
				{
					nodeTransformList.emplace_back(insert);
				}
				// 自身の子階層も同じように整列させる
				for (auto & child : transNode.lock()->nodeChilds)
				{
					InsertChildNodeTransform(child.lock(), childItr);
				}
			}

			isSort = true;	// ソートオン
		}
		ImGui::EndDragDropTarget();
	}

	bool isTreeOpenFrame = false;
	if (parent.lock()->nodeChilds.size() > 0)
	{
		ImGui::SetNextTreeNodeOpen(beforeOpen);
		parent.lock()->isTreeNodeOpen = ImGui::TreeNodeEx((std::string("##NodeTransform") + strId).c_str());
		if (beforeOpen != parent.lock()->isTreeNodeOpen)
			isTreeOpenFrame = true;
		ImGui::SameLine();
	}
	else
	{
		parent.lock()->isTreeNodeOpen = false;
	}
	
	DrawGameObject(parent, strId, popupName);

	// 階層が変化したフレームはノードをいじらない
	if ((isTreeOpenFrame || beforeOpen == parent.lock()->isTreeNodeOpen)
		&& parent.lock()->isTreeNodeOpen)
	{
		cnt++;
		for (auto & child : parent.lock()->nodeChilds)
		{
			DrawNodeChilid(child, cnt, popupName);
			cnt++;
		}
		ImGui::TreePop();
	}
}

void EditorScene::DrawGameObject(std::weak_ptr<NodeTransform> node, std::string strId, const char* popupName)
{
	std::shared_ptr<GameObject> object = node.lock()->transform.lock()->gameObject.lock();

	// オブジェクト名
	if (ImGui::Selectable((object->name + strId).c_str(), node.lock()->isSelect))
	{
		// 追加選択
		if (Keyboad::IsPress(VK_LCONTROL))
		{
			if (!node.lock()->isSelect)
				AddSelectableObject(node);
			else
				RemoveSelectableObject(node);
		}
		// 複数連続選択
		else if(Keyboad::IsPress(VK_LSHIFT))
		{
			if (selectNodeTransformList.size() == 0)
			{
				if (!node.lock()->isSelect)
					AddSelectableObject(node);
			}
			else
			{
				std::list<std::weak_ptr<NodeTransform>> shiftNodeList;
				std::list<std::shared_ptr<NodeTransform>>::iterator beginItr;
				std::list<std::shared_ptr<NodeTransform>>::iterator endItr;
				GetItrNodeTransform(selectNodeTransformList.front().lock(), beginItr);
				GetItrNodeTransform(node, endItr);
				bool isHit = false;
				// まずは下を検索
				for (auto itr = beginItr, end = nodeTransformList.end(); itr != end; ++itr)
				{
					// 親がいる場合はノードが開かれていない場合は追加しない
					if (!(*itr)->nodeParent.expired())
					{
						if((*itr)->nodeParent.lock()->isTreeNodeOpen)
							shiftNodeList.emplace_back(*itr);
					}
					else
						shiftNodeList.emplace_back(*itr);

					if (*itr == *endItr)
					{
						isHit = true;
						break;
					}
				}
				// 下になかったら上を探す
				if (!isHit)
				{
					shiftNodeList.clear();
					for (auto itr = beginItr, end = nodeTransformList.begin(); ; --itr)
					{
						// 親がいる場合はノードが開かれていない場合は追加しない
						if (!(*itr)->nodeParent.expired())
						{
							if ((*itr)->nodeParent.lock()->isTreeNodeOpen)
								shiftNodeList.emplace_back(*itr);
						}
						else
							shiftNodeList.emplace_back(*itr);

						if (*itr == *endItr)
						{
							isHit = true;
							break;
						}
					}
				}
				// 最終的に見つかったものを追加
				for (auto & add : shiftNodeList)
				{
					AddSelectableObject(add);
				}
			}
		}
		else
		{
			ClearSelectableObject();
			AddSelectableObject(node);

			// インスペクタのオブジェクトを登録
			inspectorTransform = node;
		}
	}
	// 右クリック時のポップアップ用の選択オブジェクト
	if (ImGui::IsItemHovered())
	{
		if (Mouse::IsTrigger(Mouse::Button::Right))
		{
			selectPopupObject = node;
		}
		if (Mouse::IsTrigger(Mouse::Button::Left))
		{
			isToutchHierarchyObject = true;
			/*if (!Keyboad::IsPress(VK_LCONTROL) && !Keyboad::IsPress(VK_LSHIFT))
			{
				ClearSelectableObject();
			}*/
		}
	}

	// リストごとドラッグする
	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
	{
		static std::list<std::weak_ptr<NodeTransform>> dragList;
		dragList = selectNodeTransformList;

		isDragDrop = true;

		// 複数選択していなかった場合は現在ドラッグするデータだけにする
		if (dragList.size() <= 1)
		{
			dragList.clear();
			dragList.emplace_back(node);
		}
		// 既に選択されていたらリストに存在するので追加しない
		else if(!node.lock()->isSelect)
		{
			dragList.emplace_back(node);
		}

		ImGui::SetDragDropPayload(NodeTransform::NODE_TRANSFORM_LIST, &dragList, sizeof(dragList));
		if(dragList.size() == 1)
			ImGui::Text(dragList.front().lock()->transform.lock()->gameObject.lock()->name.c_str());
		else
			ImGui::Text("etc...");

		ImGui::EndDragDropSource();
	}

	// ここに入ると親子関係
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(NodeTransform::NODE_TRANSFORM_LIST))
		{
			auto & payload_n = *(std::list<std::weak_ptr<NodeTransform>>*)payload->Data;
			std::list<std::shared_ptr<NodeTransform>>::iterator parentItr;
			GetItrNodeTransform(node, parentItr);
			auto childItr = parentItr;
			++childItr;
			
			// 階層構造になるように並び替えをする
			for (auto & transNode : payload_n)
			{
				// これから子になるオブジェクトが既に親子関係なら何もしない
				if (IsChildNodeTransform(transNode, node))
					continue;

				// 親子関係と結ぶ
				
				if (Keyboad::IsPress('L'))
				{
					transNode.lock()->transform.lock()->SetParent(node.lock()->transform, SetType::Local);
				}
				else
				{
					transNode.lock()->transform.lock()->SetParent(node.lock()->transform, SetType::World);
				}


				// 親になるオブジェクトのツリーを開く
				node.lock()->isTreeNodeOpen = true;

				while (childItr != nodeTransformList.end() && 
					(*childItr)->nodeParent.lock() == (*parentItr) && 
					*childItr != transNode.lock())
				{
					++childItr;
				}

				// 自身のイテレーターなら何もせず次へ
				if (childItr != nodeTransformList.end() && *childItr == transNode.lock())
				{
					++childItr;
					continue;
				}

				std::shared_ptr<NodeTransform> insert;
				EraseNodeTransform(transNode, insert);

				if (childItr != nodeTransformList.end())
				{
					childItr = nodeTransformList.insert(childItr, insert);
					++childItr;
				}
				else
				{
					nodeTransformList.emplace_back(insert);
				}
				// 自身の子階層も同じように整列させる
				for (auto & child : transNode.lock()->nodeChilds)
				{
					InsertChildNodeTransform(child.lock(), childItr);
				}
			}

			isSort = true;	// ソートオン
		}
		ImGui::EndDragDropTarget();
	}
}

void EditorScene::DrawInspector()
{
	if (ImGui::Begin("Inspector"))
	{
		InspectorGameObject();
	}
	ImGui::End();
}

void EditorScene::InspectorGameObject()
{
	if (inspectorTransform.expired()) return;

	const char* popupName1 = "Popup##Inspector";
	const char* popupName2 = "Popup_AddComponent##Inspector";

	auto & object = inspectorTransform.lock()->transform.lock()->gameObject;
	object.lock()->DrawImGui();

	int cnt = 0;
	std::string componentLabel = "##Component";
	
	for (auto & com : object.lock()->components)
	{
		std::string strId = componentLabel + std::to_string(cnt);

		ImGui::Checkbox(("##" + com.lock()->ClassName() + strId).c_str(), &com.lock()->Enable());

		ImGui::SameLine();
		bool isOnMouse = false;
		if (ImGui::TreeNode((com.lock()->ClassName() + strId).c_str()))
		{
			isOnMouse = ImGui::IsItemHovered();

			if ((com.lock()->ClassName().c_str(), ""))
			{
				com.lock()->DrawImGui(cnt);
			}
			ImGui::TreePop();
		}
		else
		{
			isOnMouse = ImGui::IsItemHovered();
		}

		if (isOnMouse)
		{
			if (Mouse::IsTrigger(Mouse::Button::Right))
			{
				ImGui::OpenPopup(popupName1);
				if (typeid(*com.lock()) != typeid(Transform))
					selectComponent = com;
			}
		}
		cnt++;
	}

	ImGui::Separator();
	//ImGui::SameLine();

	if (ImGui::BeginPopup(popupName1))
	{
		if (ImGui::MenuItem("Copy##Inspector"))
		{

		}
		if (ImGui::MenuItem("Paste##Inspector"))
		{

		}
		if (ImGui::MenuItem("Remove##Inspector"))
		{
			object.lock()->RemoveComponent(selectComponent);
			selectComponent.reset();
		}
		ImGui::EndPopup();
	}

	if (ImGui::Button("AddComponent", ImVec2(ImGui::GetWindowWidth() * 0.95f, 25.0f)))
	{
		ImGui::OpenPopup(popupName2);
	}

	if (ImGui::BeginPopup(popupName2))
	{
		ImGui::BeginChild("child##AddComponent", ImVec2(250, 200), true);
		for (auto & list : componentList.list)
		{
			if (ImGui::MenuItem((list->name + std::string("##AddComponent")).c_str()))
			{
				ImGui::CloseCurrentPopup();
				object.lock()->transform.lock()->gameObject.lock()->AddComponent(list->instantiate());
			}
		}
		ImGui::EndChild();

		ImGui::EndPopup();
	}
}

void EditorScene::InspectorMaterial()
{
	//std::weak_ptr<Material> & selectMaterial = Singleton<MaterialManager>::Instance()->selectMaterial;


}

void EditorScene::AddSelectableObject(std::weak_ptr<NodeTransform> object)
{
	if (object.lock()->isSelect)
		return;
	object.lock()->isSelect = true;
	selectNodeTransformList.emplace_back(object);
}

void EditorScene::RemoveSelectableObject(std::weak_ptr<NodeTransform> node)
{
	for (auto itr = selectNodeTransformList.begin(), end = selectNodeTransformList.end(); itr != end; ++itr)
	{
		if (itr->lock() == node.lock()) {
			itr->lock()->isSelect = false;
			selectNodeTransformList.erase(itr);
			return;
		}
	}
}

void EditorScene::ClearSelectableObject()
{
	for (auto & select : selectNodeTransformList)
	{
		select.lock()->isSelect = false;
	}

	selectNodeTransformList.clear();
}

void EditorScene::SortNodeTransform()
{
	std::list<std::shared_ptr<NodeTransform>> sortList;

	for (auto itr = nodeTransformList.begin(),end = nodeTransformList.end();itr != end;++itr)
	{
		// 親がいたら何もしない
		auto parent = (*itr)->transform.lock()->GetParent();
		if (!parent.expired() && !parent.lock()->gameObject.expired())
			continue;

		(*itr)->nodeParent.reset();
		sortList.emplace_back(*itr);
		AddChildNodeTransform(sortList, *itr);
	}

	nodeTransformList = sortList;
	std::list<std::shared_ptr<NodeTransform>>().swap(sortList);

	// ソートしたオブジェクトをシーンデータに反映
	std::list<std::weak_ptr<GameObject>>().swap(sceneData->gameObjectList);
	for (auto node : nodeTransformList)
	{
		std::weak_ptr<GameObject> sceneObjct = node->transform.lock()->gameObject;
		sceneData->gameObjectList.push_back(sceneObjct);
	}
}

void EditorScene::GetItrNodeTransform(std::weak_ptr<NodeTransform> object, std::list<std::shared_ptr<NodeTransform>>::iterator & out)
{
	for (auto itr = nodeTransformList.begin(), end = nodeTransformList.end(); itr != end; ++itr)
	{
		if (object.lock() == *itr)
		{
			out = itr;
			return;
		}
	}
}

void EditorScene::EraseNodeTransform(std::weak_ptr<NodeTransform> object)
{
	for (auto itr = nodeTransformList.begin(), end = nodeTransformList.end(); itr != end; ++itr)
	{
		if (object.lock() == *itr)
		{
			nodeTransformList.erase(itr);
			break;
		}
	}
}

void EditorScene::EraseNodeTransform(std::weak_ptr<NodeTransform> object, std::shared_ptr<NodeTransform> & out)
{
	for (auto itr = nodeTransformList.begin(), end = nodeTransformList.end(); itr != end; ++itr)
	{
		if (object.lock() == *itr)
		{
			out = *itr;
			nodeTransformList.erase(itr);
			break;
		}
	}
}

void EditorScene::InsertChildNodeTransform(std::shared_ptr<NodeTransform> object, std::list<std::shared_ptr<NodeTransform>>::iterator & inItr)
{
	// 一旦外す
	EraseNodeTransform(object);

	std::list<std::shared_ptr<NodeTransform>>::iterator nextItr = nodeTransformList.end();
	if (inItr != nextItr)
	{
		nextItr = nodeTransformList.insert(inItr, object);
		++nextItr;
	}
	else
	{
		nodeTransformList.emplace_back(object);
	}

	for (auto & child : object->nodeChilds)
	{
		InsertChildNodeTransform(child.lock(), nextItr);
	}
}

void EditorScene::AddChildNodeTransform(std::list<std::shared_ptr<NodeTransform>>& nodeList, std::shared_ptr<NodeTransform> & parent)
{
	// 一旦リセット
	parent->nodeChilds.clear();

	std::list<std::weak_ptr<Transform>> & childs = parent->transform.lock()->GetChilds();

	// 親のポインタを入手
	for (auto & child : childs)
	{
		// ボーンなら追加しない
		if (child.lock()->gameObject.expired())
			continue;

		// ノードリストからイテレーターを取得
		auto hitItr = std::find_if(nodeTransformList.begin(), nodeTransformList.end(), [child](std::shared_ptr<NodeTransform> & com)
		{
			return child.lock() == com->transform.lock();
		});
		if (hitItr == nodeTransformList.end()) continue;
		
		nodeList.emplace_back(*hitItr);
		parent->nodeChilds.emplace_back(*hitItr);

		std::shared_ptr<NodeTransform> & nodeBack = nodeList.back();
		nodeBack->nodeParent = parent;

		AddChildNodeTransform(nodeList, nodeBack);
	}
}

bool EditorScene::IsChildNodeTransform(std::weak_ptr<NodeTransform> parent, std::weak_ptr<NodeTransform> checkChild)
{
	for (auto & child : parent.lock()->nodeChilds)
	{
		if (IsChildNodeTransform(child, checkChild))
			return true;
	}

	return parent.lock() == checkChild.lock();
}

void EditorScene::OpneTreeNodeTransformChilds(std::weak_ptr<NodeTransform> node)
{
	node.lock()->isTreeNodeOpen = true;

	for (auto child : node.lock()->nodeChilds)
	{
		OpneTreeNodeTransformChilds(child);
	}
}
