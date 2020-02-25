#ifndef _EDITOR_SCENE_H_
#define _EDITOR_SCENE_H_

#include "../../FrameWork/Common.h"
#include "../../DirectX/Common.h"
#include "../../FrameWork/Component/Transform/NodeTransform.h"

#include <functional>
#include <memory>
#include <sstream>

class EditorScene : public SceneBase
{
public:
	EditorScene();
	virtual ~EditorScene();

	void Initialize() override;
	void Start() override;
	void Update() override;
	void Draw() override;
private:

	enum class State : int
	{
		Stop,			// 静止中
		Play,			// ゲーム中
	};

	enum class Mode : int
	{
		Edit,
		Game
	};

	enum ManipulatorType
	{
		Translate,
		Scale,
		Rotate
	};

	enum AxisType
	{
		Forward,
		Right,
		Up,
		All,
		NUM
	};

	State state;
	bool isInterruption = false;	// 中断
	Mode mode;

	bool isToutchHierarchyObject = false;
	bool isDrag = false;
	bool isDragDrop = false;
	bool isSort = false;			// ソートするタイミングの時にオンにする
	bool isMoveToModel = false;		// オンになると選択中のオブジェクトがカメラの視界に入るまで追従する
	AxisType axisType;
	Vector3 beforePos;
	Vector3 beforeSca;
	Vector3 axisDir;
	ManipulatorType manipulatorType;

	std::shared_ptr<GameObject> editorCameraObject;	// エディター用のオブジェクト
	std::list<std::shared_ptr<Component>> editorComponentList;
	std::weak_ptr<DXCamera> editorCamera;			// エディター用のカメラコンポーネント

	std::shared_ptr<GameObject> cubeManipulator;			// 移動、スケール用の中心メッシュ
	std::shared_ptr<GameObject> arrowManipulator;			// 移動用のメッシュ

	Matrix4 manipuratorMatrix; // マニピュレーターのマトリクス
	std::vector<std::weak_ptr<Transform>> manipuratorChilds;

	Color axisColorArray[AxisType::NUM];
	Matrix4 axisMatrixArray[AxisType::NUM];

	std::unique_ptr<std::stringstream> editSceneData;			// 実行直前のシーンデータ

	std::list<std::shared_ptr<NodeTransform>> nodeTransformList;
	std::list<std::weak_ptr<NodeTransform>> selectNodeTransformList;		// 選択しているオブジェクト
	std::list <std::unique_ptr<std::stringstream>> copyDataList;			// コピーリスト
	std::list <std::string> pastObjectNameList;								// ペーストしたオブジェクトを登録しておくリスト
	std::weak_ptr<NodeTransform> curSelectParentNode;						// 現在選択している先頭のノードにある親を指すポインタ（いない場合は空）
	std::weak_ptr<Component> selectComponent;
	std::weak_ptr<NodeTransform> selectPopupObject;

	void CreateComponentList();
	void CreateEditorCamera();
	void CreateManipulator();
	void SetAxisColor(AxisType type, const Color & color);
	void DrawScene();
	void DrawImGui();
	void DrawManipulator();
	void DrawHierarchy();
	void DrawNodeChilid(std::weak_ptr<NodeTransform> parent, int& cnt, const char* popupName);
	void DrawGameObject(std::weak_ptr<NodeTransform> node, std::string strId, const char* popupName);
	void DrawInspector();
	void InspectorGameObject();
	void InspectorMaterial();
	void AddSelectableObject(std::weak_ptr<NodeTransform> object);
	void RemoveSelectableObject(std::weak_ptr<NodeTransform> object);
	void ClearSelectableObject();
	void SortNodeTransform();
	void GetItrNodeTransform(std::weak_ptr<NodeTransform> object, std::list<std::shared_ptr<NodeTransform>>::iterator & out);
	void EraseNodeTransform(std::weak_ptr<NodeTransform> object);
	void EraseNodeTransform(std::weak_ptr<NodeTransform> object, std::shared_ptr<NodeTransform> & out);
	void InsertChildNodeTransform(std::shared_ptr<NodeTransform> object, std::list<std::shared_ptr<NodeTransform>>::iterator & inItr);
	void AddChildNodeTransform(std::list<std::shared_ptr<NodeTransform>> & nodeList, std::shared_ptr<NodeTransform> & parent);
	bool IsChildNodeTransform(std::weak_ptr<NodeTransform> parent, std::weak_ptr<NodeTransform> checkNode);
	void OpneTreeNodeTransformChilds(std::weak_ptr<NodeTransform> node);	// 引数のノードの子階層を全てオープンする
};

#endif // !_EDITOR_SCENE_H_