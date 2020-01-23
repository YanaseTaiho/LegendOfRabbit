#include "Transform.h"
#include "../../GameObject/GameObject.h"
#include "../Rigidbody/Rigidbody.h"

using namespace FrameWork;

std::function<void(std::weak_ptr<Transform>)> Transform::AddTransform;
std::function<void(std::weak_ptr<Transform>)> Transform::RemoveTransform;

void Transform::DrawImGui(int id)
{
	Vector3 pos = GetLocalPosition();
	if (ImGui::DragFloat3("Position", pos, 0.05f))
		SetLocalPosition(pos);
	Vector3 scale = GetLocalScale();
	if (ImGui::DragFloat3("Scale", scale, 0.05f))
		SetLocalScale(scale);

	Vector3 rot = GetLocalRotation().GetEulerAngles();
	Vector3 inRot = rot;
	if (ImGui::DragFloat3("Rotation", inRot, 0.05f))
	{
		SetLocalRotation(Quaternion::EulerAngles(inRot));
	}
}

Transform::Transform()
{
	SetLocalTransform(Vector3::zero(), Vector3::one(), Quaternion::Identity());
}

Transform::Transform(const Transform & trans)
{
	this->translation = trans.translation;
	this->scaling = trans.scaling;
	this->rotate = trans.rotate;
	this->localMatrix = trans.localMatrix;
	this->worldMatrix = trans.worldMatrix;
}

Transform::~Transform() 
{
	
}

void Transform::Update()
{
	// ���[�J���}�g���N�X���Z�b�g
	SetLocalMatrix();
	// ���[���h�}�g���N�X���Z�b�g( �e������ꍇ�e�̉e�����󂯂� )
	SetWorldMatrix();
}

void Transform::SetWorldTransform(const Vector3 & position, const Vector3 & scale, const Quaternion & rotation)
{
	SetWorldPosition(position);
	SetWorldScale(scale);
	SetWorldRotation(rotation);
}

void Transform::SetLocalTransform(const Vector3 & position, const Vector3 & scale, const Quaternion & rotation)
{
	translation = Translation3f(position.x, position.y, position.z);
	scaling = Scaling3f(scale.x, scale.y, scale.z);
	rotate = rotation;
	SetLocalMatrix();
	SetWorldMatrix();
}

void Transform::SetWorldPosition(const Vector3 & position)
{
	// �e�����Ȃ��ꍇ
	if (parent.expired())
	{
		SetLocalPosition(position);
	} 
	// �e������ꍇ
	else
	{
		// ���[���h���W�ɒ��ڃ|�W�V�������w��
		worldMatrix.matrix(0, 3) = position.x;
		worldMatrix.matrix(1, 3) = position.y;
		worldMatrix.matrix(2, 3) = position.z;
		// �ݒ肵�����[���h�}�g���N�X��e�̋t�s����|���ă��[�J���}�g���N�X�ɕϊ�
		localMatrix.matrix = parent.lock()->worldMatrix.matrix.inverse() * worldMatrix.matrix;
		Vector3 localPosition = localMatrix.position();
		Vector3 localScale = localMatrix.scale();
		Quaternion localRotation = localMatrix.rotation().Normalized();
		this->translation = Translation3f(localPosition.x, localPosition.y, localPosition.z);
		this->scaling = Scaling3f(localScale.x, localScale.y, localScale.z);
		this->rotate = localRotation;

		SetWorldMatrix();
	}
}

void Transform::SetLocalPosition(const Vector3 & position)
{
	translation = Translation3f(position.x, position.y, position.z); //XMMatrixTranslation(position.x, position.y, position.z);
	SetLocalMatrix();
	SetWorldMatrix();
}

void Transform::SetWorldScale(const Vector3 & scale)
{
	// �e�����Ȃ��ꍇ
	if (parent.expired())
	{
		SetLocalScale(scale);
	}
	// �e������ꍇ
	else
	{
		// �e�̃X�P�[���̋t�s����|����
		Scaling3f s = Scaling3f(scale.x, scale.y, scale.z);
		Vector3 parentScale = parent.lock()->GetWorldScale();
		Scaling3f sInv = Scaling3f(parentScale.x, parentScale.y, parentScale.z).inverse();
		scaling = ScalingMultiply(s, sInv);
		SetLocalMatrix();
		SetWorldMatrix();
	}
}

void Transform::SetLocalScale(const Vector3 & scale)
{
	scaling = Scaling3f(scale.x, scale.y, scale.z);
	SetLocalMatrix();
	SetWorldMatrix();
}

void Transform::SetWorldRotation(const Quaternion & rotation)
{
	// �e�����Ȃ��ꍇ
	if (parent.expired())
	{
		SetLocalRotation(rotation);
	}
	// �e������ꍇ
	else
	{
		// �e�̉�]�s��̋t�s����|����
		Quaternion inverse = parent.lock()->worldMatrix.rotation().Inversed();
		rotate = inverse * rotation;
		SetLocalMatrix();
		SetWorldMatrix();
	}
}

void Transform::SetLocalRotation(const Quaternion & rotation)
{
	rotate = rotation;
	SetLocalMatrix();
	SetWorldMatrix();
}

void Transform::SetLocalMatrix(const Matrix4 & matrix)
{
	localMatrix = matrix;
	Vector3 localPosition = localMatrix.position();
	Vector3 localScale = localMatrix.scale();
	Quaternion localRotation = localMatrix.rotation();
	this->translation = Translation3f(localPosition.x, localPosition.y, localPosition.z);
	this->scaling = Scaling3f(localScale.x, localScale.y, localScale.z);
	this->rotate = localRotation;
}

void Transform::SetLocalMatrix(const Vector3 & position, const Vector3 & scale, const Quaternion & rotation)
{
	this->translation = Translation3f(position.x, position.y, position.z);
	this->scaling = Scaling3f(scale.x, scale.y, scale.z);
	this->rotate = rotation;
	SetLocalMatrix();
}

void Transform::SetWorldMatrix(const Matrix4 & matrix)
{
	worldMatrix = matrix;
}

Vector3 Transform::GetWorldPosition()
{
	return worldMatrix.position();
}

Vector3 Transform::GetLocalPosition()
{
	return Vector3(translation.x(), translation.y(), translation.z());
}

Vector3 Transform::GetWorldScale()
{
	Vector3 lossy = GetLocalScale();
	if(!parent.expired())
		parent.lock()->LossyScale(lossy);

	return lossy;
}

Vector3 Transform::GetLocalScale()
{
	return Vector3(scaling.diagonal().x(), scaling.diagonal().y(), scaling.diagonal().z());
}

Quaternion Transform::GetWorldRotation()
{
	return worldMatrix.rotation();
}

Quaternion Transform::GetLocalRotation()
{
	return rotate;
}

void Transform::SetParent(std::weak_ptr<Transform>& parent, SetType type, int number)
{
	// ���������g�Ȃ牽�����Ȃ� or ���ɐe�q�֌W�Ȃ牽�����Ȃ� 
	if (parent.lock() == transform.lock() || this->parent.lock() == parent.lock()) return;

	// ���ɑ��Ɛe�q�֌W�ɂȂ��Ă��鎞���̐e�Ɖ���؂�
	if (!this->parent.expired())
		this->ReleaseParent();

	this->parent = parent;
	// �e�̎q�Ɏ��g��ǉ�
	auto & childs = this->parent.lock()->childs;
	if (number < 0 || number >= childs.size())
	{
		childs.emplace_back(transform);
	}
	else
	{
		auto itr = childs.begin();
		while (number > 0)
		{
			number--;
			itr++;
		}
		this->parent.lock()->childs.insert(itr, transform);
	}

	this->parent.lock()->ResetRigidbodyCollision();

	// ���[���h�ݒ�̏ꍇ�̂�
	if (type == SetType::World)
	{
		// �e�ɂȂ�g�����X�t�H�[���̋t�s����|����
		localMatrix = this->parent.lock()->worldMatrix.Inverse() * this->worldMatrix;
	    Vector3 worldParentScale = this->parent.lock()->GetWorldScale();

		Vector3 worldPosition = localMatrix.position();
		Vector3 worldScale = GetLocalScale();
		Quaternion worldRotation = localMatrix.rotation();
		worldScale.x = (worldParentScale.x == 0.0f) ? 0.0f : worldScale.x / worldParentScale.x;
		worldScale.y = (worldParentScale.y == 0.0f) ? 0.0f : worldScale.y / worldParentScale.y;
		worldScale.z = (worldParentScale.z == 0.0f) ? 0.0f : worldScale.z / worldParentScale.z;

		this->translation = Translation3f(worldPosition.x, worldPosition.y, worldPosition.z);
		this->scaling = Scaling3f(worldScale.x, worldScale.y, worldScale.z);
		this->rotate = worldRotation;
	}
	this->SetWorldMatrix();
}

void Transform::SetChild(std::weak_ptr<Transform>& child, SetType type, int number)
{
	// �q�ɂȂ�g�����X�t�H�[���̐e�Ɏ��g���Z�b�g
	child.lock()->SetParent(transform, type, number);
}

std::weak_ptr<Transform> Transform::GetParent()
{
	if (this->parent.expired())
		return std::weak_ptr<Transform>();
	else
		return this->parent;
}

std::weak_ptr<Transform> Transform::GetBossParent()
{
	if (this->parent.expired())
		return transform;

	return GetBossParent();
}

std::weak_ptr<Transform> Transform::GetChild(int num)
{
	if (childs.size() < num || num < 0) 
		return std::weak_ptr<Transform>();

	for (std::weak_ptr<Transform> & child : childs)
	{
		if (num == 0 && !child.expired())
		{
			return child;
		}
		num--;
	}

	return std::weak_ptr<Transform>();
}

std::weak_ptr<Transform> Transform::GetChild(std::string name)
{
	for (std::weak_ptr<Transform> & child : childs)
	{
		if (child.lock()->gameObject.expired()) continue;
		if (child.lock()->gameObject.lock()->name == name)
		{
			return child;
		}
	}
	return std::weak_ptr<Transform>();
}

void Transform::ReleaseParent()
{
	if (this->parent.expired()) return;
	// �e�̎q���X�g���玩�g���폜
	this->parent.lock()->PrivateReleaseChild(this->transform);
	// ���g�̐e���폜
	this->PrivateReleaseParent();
}

void Transform::ReleaseChild(int num)
{
	if (num < 0 || childs.size() <= num) return;

	for (auto itr = childs.begin(), end = childs.end(); itr != end; ++itr)
	{
		if (num > 0)
		{
			num--;
			continue;
		}

		// �폜����q�̐e��؂藣��
		itr->lock()->PrivateReleaseParent();
		// ���g�̎q���X�g����^�[�Q�b�g���폜
		childs.erase(itr);
		// �O������ɃR���W�������Z�b�g������
		ResetRigidbodyCollision();
		return;
	}
}

void Transform::ReleaseChild(std::weak_ptr<Transform> child)
{
	if (child.expired()) return;
	
	for (auto itr = childs.begin(), end = childs.end(); itr != end; ++itr)
	{
		if (itr->expired()) continue;
		if (itr->lock() == child.lock())
		{
			// �폜����q�̐e��؂藣��
			itr->lock()->PrivateReleaseParent();
			// ���g�̎q���X�g����^�[�Q�b�g���폜
			childs.erase(itr);
			// �O������ɃR���W�������Z�b�g������
			ResetRigidbodyCollision();
			return;
		}
	}
}

void Transform::ReleaseChild(std::weak_ptr<GameObject> child)
{
	if (childs.size() == 0) return;

	ReleaseChild(child.lock()->transform);
}

void Transform::ReleaseChild(std::string name)
{
	if (childs.size() == 0) return;

	auto end = childs.end();
	for (auto itr = childs.begin(); itr != end; ++itr)
	{
		if (itr->lock()->gameObject.expired()) continue;
		if (itr->lock()->gameObject.lock()->name == name)
		{
			// �폜����q�̐e��؂藣��
			itr->lock()->PrivateReleaseParent();
			// ���g�̎q���X�g����^�[�Q�b�g���폜
			childs.erase(itr);
			// �O������ɃR���W�������Z�b�g������
			ResetRigidbodyCollision();
			return;
		}
	}
}

void Transform::ReleaseChildAll()
{
	if (childs.size() == 0) return;

	for (auto & child : childs)
	{
		// �폜����q�̐e��؂藣��
		child.lock()->PrivateReleaseParent();
	}
	// �q�̎Q�Ƃ�S�ăN���A
	childs.clear();

	// �O������ɃR���W�������Z�b�g������
	ResetRigidbodyCollision();
}

Vector3 Transform::forward()
{
	return worldMatrix.forward();
}

Vector3 Transform::right()
{
	return worldMatrix.right();
}

Vector3 Transform::up()
{
	return worldMatrix.up();
}

Matrix4 Transform::GetWorldMatrix()
{
	return worldMatrix;
}

Matrix4 Transform::GetLocalMatrix()
{
	return localMatrix;
}

void Transform::AddComponent()
{
	Component::AddComponent();
	transform = std::static_pointer_cast<Transform>(allComponentList.back());	// �����̃I�u�W�F�N�g�̎Q�Ƃ�n��
	gameObject.lock()->SetTransform(transform);	

	if(AddTransform && !gameObject.expired())
		AddTransform(transform);

	Awake();
}

void Transform::RemoveComponent()
{
	Finalize();

	if (RemoveTransform)
		RemoveTransform(transform);

	Component::RemoveComponent();
}

void Transform::SetLocalMatrix()
{
	Eigen::Affine3f affine;
	affine = translation * rotate.quaternion * scaling;
	localMatrix.matrix = affine.matrix();
}

void Transform::SetWorldMatrix()
{
	// �e�����Ȃ��ꍇ
	if (parent.expired())
	{
		worldMatrix = localMatrix;
	}
	// �e������ꍇ
	else
	{
		worldMatrix = parent.lock()->worldMatrix * localMatrix;
	}

	// �q������ꍇ�͎q�̃��[���h�s��̍X�V
	UpdateChilds();
}

void Transform::DestroyChilds()
{
	for (auto & child : childs)
	{
		if (child.lock()->gameObject.expired()) continue;
		child.lock()->DestroyChilds();
		child.lock()->gameObject.lock()->OnDestroy();
	}
	ReleaseChildAll();
}

void Transform::PrivateReleaseParent()
{
	if (this->parent.expired()) return;

	Vector3 worldPosition = GetWorldPosition();
	Vector3 worldScale = GetWorldScale();
	Quaternion worldRotation = GetWorldRotation();
	// ���g�̐e���폜
	this->parent.reset();

	// ���[�J����Ԃ̒l�����[���h��Ԃ̒l�ɕϊ�
	translation = Translation3f(worldPosition.x, worldPosition.y, worldPosition.z);
	scaling = Scaling3f(worldScale.x, worldScale.y, worldScale.z);
	rotate = worldRotation;
	SetLocalMatrix();
	SetWorldMatrix();
}

void Transform::PrivateReleaseChild(std::weak_ptr<Transform> & child)
{
	for (auto itr = childs.begin(), end = childs.end(); itr != end; ++itr)
	{
		if (itr->lock() == child.lock())
		{
			// ���g�̎q���X�g����^�[�Q�b�g���폜
			childs.erase(itr);
			ResetRigidbodyCollision();
			return;
		}
	}
}

void Transform::ResetRigidbodyCollision()
{
	if (gameObject.expired()) return;

	auto rigid = gameObject.lock()->GetComponent<Rigidbody>();
	if (rigid.expired())
		rigid = gameObject.lock()->GetComponentWithParent<Rigidbody>();
	if (rigid.expired()) return;

	rigid.lock()->ResetCollisions();
}

void Transform::LossyScale(Vector3 & s)
{
	s *= GetLocalScale();
	if (!parent.expired())
		parent.lock()->LossyScale(s);
}

void Transform::UpdateChilds()
{
	for (auto & child : this->childs)
	{
		child.lock()->SetWorldMatrix();
	}
}

void Transform::Finalize()
{
	ReleaseParent(); // ���g�Ɛe�Ƃ̊֌W����菜��
	DestroyChilds(); // �q��������O�ɍ폜����
}

void Transform::LoadTransform()
{
	if (AddTransform && !gameObject.expired())
		AddTransform(transform);

	if (parent.expired())
		worldMatrix = localMatrix;
	else
		worldMatrix = parent.lock()->worldMatrix * localMatrix;
}

Scaling3f Transform::ScalingMultiply(const Scaling3f & a, const Scaling3f & b)
{
	return Scaling3f(a.diagonal().x() * b.diagonal().x(), a.diagonal().y() * b.diagonal().y(), a.diagonal().z() * b.diagonal().z());
}
