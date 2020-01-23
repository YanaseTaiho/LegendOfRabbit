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
		Stop,			// �Î~��
		Play,			// �Q�[����
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
	bool isInterruption = false;	// ���f
	Mode mode;

	bool isToutchHierarchyObject = false;
	bool isDrag = false;
	bool isDragDrop = false;
	bool isSort = false;			// �\�[�g����^�C�~���O�̎��ɃI���ɂ���
	bool isMoveToModel = false;		// �I���ɂȂ�ƑI�𒆂̃I�u�W�F�N�g���J�����̎��E�ɓ���܂ŒǏ]����
	AxisType axisType;
	Vector3 beforePos;
	Vector3 beforeSca;
	Vector3 axisDir;
	ManipulatorType manipulatorType;

	std::shared_ptr<GameObject> editorCameraObject;	// �G�f�B�^�[�p�̃I�u�W�F�N�g
	std::list<std::shared_ptr<Component>> editorComponentList;
	std::weak_ptr<DXCamera> editorCamera;			// �G�f�B�^�[�p�̃J�����R���|�[�l���g

	std::shared_ptr<GameObject> cubeManipulator;			// �ړ��A�X�P�[���p�̒��S���b�V��
	std::shared_ptr<GameObject> arrowManipulator;			// �ړ��p�̃��b�V��

	Matrix4 manipuratorMatrix; // �}�j�s�����[�^�[�̃}�g���N�X
	std::vector<std::weak_ptr<Transform>> manipuratorChilds;

	Color axisColorArray[AxisType::NUM];
	Matrix4 axisMatrixArray[AxisType::NUM];

	std::unique_ptr<std::stringstream> editSceneData;			// ���s���O�̃V�[���f�[�^

	std::list<std::shared_ptr<NodeTransform>> nodeTransformList;
	std::list<std::weak_ptr<NodeTransform>> selectNodeTransformList;		// �I�����Ă���I�u�W�F�N�g
	std::weak_ptr<NodeTransform> inspectorTransform;						// �C���X�y�N�^�ɕ\������I�u�W�F�N�g
	std::list <std::unique_ptr<std::stringstream>> copyDataList;			// �R�s�[���X�g
	std::list <std::string> pastObjectNameList;								// �y�[�X�g�����I�u�W�F�N�g��o�^���Ă������X�g
	std::weak_ptr<NodeTransform> curSelectParentNode;						// ���ݑI�����Ă���擪�̃m�[�h�ɂ���e���w���|�C���^�i���Ȃ��ꍇ�͋�j
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
	void OpneTreeNodeTransformChilds(std::weak_ptr<NodeTransform> node);	// �����̃m�[�h�̎q�K�w��S�ăI�[�v������
};

#endif // !_EDITOR_SCENE_H_