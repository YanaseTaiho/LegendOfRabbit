#ifndef _TRANSFORM_H_
#define _TRANSFORM_H_

#include <memory>
#include <list>
#include <string>
#include <functional>
#include "../Component.h"
#include "../../Quaternion.h"
#include "../../Vector.h"
#include "../../Matrix.h"
#include "../../SetType.h"

class EditorScene;

namespace FrameWork
{
	class AnimationClip;
	class GameObjectManager;
	class Prefab;

	class Transform : public Component
	{
		friend AnimationClip;
		friend EditorScene;
		friend GameObjectManager;
		friend Prefab;

	private:
		friend cereal::access;
		template<class Archive>
		void save(Archive & archive) const
		{
			archive(cereal::make_nvp("Component", cereal::base_class<Component>(this)),
				CEREAL_NVP(translation), CEREAL_NVP(scaling), CEREAL_NVP(rotate),
				CEREAL_NVP(parent), CEREAL_NVP(childs));
		}
		template<class Archive>
		void load(Archive & archive)
		{
			archive(cereal::make_nvp("Component", cereal::base_class<Component>(this)),
				CEREAL_NVP(translation), CEREAL_NVP(scaling), CEREAL_NVP(rotate),
				CEREAL_NVP(parent), CEREAL_NVP(childs));

			SetLocalMatrix();
		}

		void LoadSerialized(const std::shared_ptr<Component> & component) override
		{
			Component::LoadSerialized(component);
			LoadTransform();
		}
		void DrawImGui(int id) override;

		static std::function<void(std::weak_ptr<Transform>)> AddTransform;
		static std::function<void(std::weak_ptr<Transform>)> RemoveTransform;

	public:
		Transform();
		Transform(const Transform & trans);
		Transform(Vector3 position, Vector3 scale = Vector3::one(), Quaternion rotation = Quaternion::Identity())
		{
			SetLocalTransform(position, scale, rotation);
		}
		~Transform();

		void SetWorldTransform(const Vector3 & position, const Vector3 & scale, const Quaternion & rotation);
		void SetLocalTransform(const Vector3 & position, const Vector3 & scale, const Quaternion &  rotation);

		void SetWorldPosition(const Vector3 & position);
		void SetLocalPosition(const Vector3 & position);
		void SetWorldScale(const Vector3 & scale);
		void SetLocalScale(const Vector3 & scale);
		void SetWorldRotation(const Quaternion &  rotation);
		void SetLocalRotation(const Quaternion &  rotation);

		void Update();
		void UpdateChilds();

		void SetLocalMatrix(const Matrix4 & matrix);
		void SetLocalMatrix(const Vector3 & position, const Vector3 & scale, const Quaternion &  rotation);
		void SetWorldMatrix(const Matrix4 & matrix);

		Vector3 GetWorldPosition();
		Vector3 GetLocalPosition();
		Vector3 GetWorldScale();
		Vector3 GetLocalScale();
		Quaternion GetWorldRotation();
		Quaternion GetLocalRotation();

		void SetParent(std::weak_ptr<Transform> & parent, SetType type = SetType::World, int number = -1);	// number : 何番目の子に追加するか
		void SetChild(std::weak_ptr<Transform> & child, SetType type = SetType::World, int number = -1);	// number : 何番目の子に追加するか

		std::weak_ptr<Transform> GetParent();		// 自身の親を返す( いなかったら空を返す )
		std::weak_ptr<Transform> GetBossParent();	// 階層の一番上の親を返す( 自身がそうなら自身を返す )
		std::weak_ptr<Transform> GetChild(int num); // 子のナンバーに一致したオブジェクトを取得
		std::weak_ptr<Transform> GetChild(std::string name); // 名前が一致した子を取得
		std::list<std::weak_ptr<Transform>> & GetChilds() { return childs; };

		bool IsChild(std::weak_ptr<Transform> other);	// 引数のオブジェクトが子に存在するかを確かめる

		void ReleaseParent();	// 親との関係を相互に断ち切る
		void ReleaseChild(int num); // num番目の子との関係を相互に断ち切る
		void ReleaseChild(std::weak_ptr<Transform> child);	// 引数の子との関係を相互に断ち切る
		void ReleaseChild(std::weak_ptr<GameObject> child);	// 引数の子との関係を相互に断ち切る
		void ReleaseChild(std::string name);	// 名前が一致した子との関係を相互に断ち切る
		void ReleaseChildAll();	// 全ての子との関係を相互に断ち切る

		Vector3 forward();	// 自身の前方を取得
		Vector3 right();	// 自身の右方を取得
		Vector3 up();		// 自身の上方を取得

		Matrix4 GetWorldMatrix();
		Matrix4 GetLocalMatrix();


	private:
		void AddComponent() override final;
		void RemoveComponent() override final;

		void SetLocalMatrix();
		void SetWorldMatrix();
		void DestroyChilds();
		void PrivateReleaseParent();
		void PrivateReleaseChild(std::weak_ptr<Transform> & child);
		void ResetRigidbodyCollision();
		void LossyScale(Vector3 & s);

		void Finalize();

		void LoadTransform();

		Scaling3f ScalingMultiply(const Scaling3f & a, const Scaling3f & b);

		std::weak_ptr<Transform> parent;
		std::list<std::weak_ptr<Transform>> childs;

		// 平行移動(x, y, z)
		Translation3f translation;
		// スケーリング
		Scaling3f scaling;
		// 回転(クォータニオン)
		Quaternion rotate;

		Matrix4 worldMatrix;
		Matrix4 localMatrix;
	};
}

CEREAL_REGISTER_TYPE(FrameWork::Transform)
//cereal::base_class<>()←これを呼べばマクロは必要ない
//CEREAL_REGISTER_POLYMORPHIC_RELATION(FrameWork::Component, FrameWork::Transform)

#endif //!_TRANSFORM_H_