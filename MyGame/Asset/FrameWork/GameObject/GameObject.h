#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

#include "../Tag.h"
#include "../Layer.h"
#include "../SetType.h"
#include "../Component/Component.h"
#include "../Component/Transform/Transform.h"
#include <memory>
#include <list>
#include <string>

class EditorScene;

namespace FrameWork
{
	class GameObjectManager;
	class SceneManager;
	class Collision;
	class Rigidbody;
	class MonoBehaviour;
	class Prefab;

	class GameObject final
	{
		friend GameObjectManager;
		friend SceneManager;
		friend Component;
		friend Transform;
		friend MonoBehaviour;
		friend Rigidbody;

		friend EditorScene;
		friend Prefab;

	private:
		friend cereal::access;
		template<class Archive>
		void save(Archive & archive) const
		{
			archive(CEREAL_NVP(isActive), CEREAL_NVP(name),CEREAL_NVP(tag), CEREAL_NVP(layer), 
				CEREAL_NVP(gameObject), CEREAL_NVP(transform), CEREAL_NVP(components), CEREAL_NVP(behaviours));
		}

		template<class Archive>
		void load(Archive & archive)
		{
			archive(CEREAL_NVP(isActive), CEREAL_NVP(name), CEREAL_NVP(tag), CEREAL_NVP(layer),
				CEREAL_NVP(gameObject), CEREAL_NVP(transform), CEREAL_NVP(components), CEREAL_NVP(behaviours));

			// 何かしらの処理を書く

		}

		void DrawImGui();

	public:
		GameObject() = default;
		GameObject(const GameObject & object);
		GameObject(std::string name, Tag tag, Layer layer);
		~GameObject();

		std::weak_ptr<GameObject> gameObject;	// 自身を指す参照ポインタ
		std::weak_ptr<Transform> transform; // トランスフォームの参照ポインタ
		
		Tag tag; // タグ
		std::string name; // 名前

		// オブジェクトが有効か無効かを設定
		void SetActive(bool active);
		// オブジェクトが有効か無効か
		bool IsActive();
		Layer GetLayer() { return layer; }
		Layer ChangeLayer(Layer layer);

		// stopFrame : オブジェクトを停止するフレーム数(0以下で設定した場合は無限に停止する)
		void SetStop(bool isStop);
		bool IsStop() { return isStop; }

		// 親をセットする
		std::weak_ptr<GameObject> SetParent(std::weak_ptr<GameObject> gameObject, SetType type = SetType::World);
		// 子をセットする
		std::weak_ptr<GameObject> SetChild(std::weak_ptr<GameObject> gameObject, SetType type = SetType::World);

		// コンポーネントを追加
		template <typename T>
		void AddComponent(T * component)
		{
			components.emplace_back(Component::AddComponent(component, gameObject, transform));
		}

		// コンポーネントを取得
		template <typename T>
		std::weak_ptr<T> GetComponent()
		{
			for (auto & component : components)
			{
				// ダイナミックキャストすることでそのコンポーネントかを判断する
				// typeidだと継承前のクラスの型で取れないため
				std::weak_ptr<T> cast = std::dynamic_pointer_cast<T>(component.lock());
				if (cast.expired()) continue;

				return cast;
			}

			return std::weak_ptr<T>();
		}

		// 親のコンポーネントを取得
		template <typename T>
		std::weak_ptr<T> GetComponentWithParent()
		{
			auto parent = transform.lock()->GetParent();
			if (!parent.expired())
			{
				auto object = parent.lock()->gameObject;
				if (!object.expired())
				{
					for (auto & component : object.lock()->components)
					{
						std::weak_ptr<T> cast = std::dynamic_pointer_cast<T>(component.lock());
						if (cast.expired()) continue;

						return cast;
					}

					return object.lock()->GetComponentWithParent<T>();
				}
			}

			return std::weak_ptr<T>();
		}

		// 子のコンポーネントを取得
		template <typename T>
		std::weak_ptr<T> GetComponentWithChild()
		{
			for (auto & child : transform.lock()->GetChilds())
			{
				auto obj = child.lock()->gameObject;
				if (obj.expired()) continue;

				for (auto & component : obj.lock()->components)
				{
					std::weak_ptr<T> cast = std::dynamic_pointer_cast<T>(component.lock());
					if (cast.expired()) continue;

					return cast;
				}

				return obj.lock()->GetComponentWithChild<T>();
			}

			return std::weak_ptr<T>();
		}

		// コンポーネント群を取得
		template <typename T>
		void GetComponents(std::list<std::weak_ptr<T>> * arrays)
		{
			for (auto & component : components)
			{
				// ダイナミックキャストすることでそのコンポーネントかを判断する
				// typeidだと継承前のクラスの型で取れないため
				std::weak_ptr<T> cast = std::dynamic_pointer_cast<T>(component.lock());
				if (cast.expired()) continue;

				arrays->emplace_back(cast);
			}
		}

		// 親のコンポーネント群を取得
		template <typename T>
		void GetComponentsWithParent(std::list<std::weak_ptr<T>> * arrays)
		{
			auto parent = transform.lock()->GetParent();
			if (!parent.expired())return;

			auto object = parent.lock()->gameObject;
			if (!object.expired()) return;

			for (auto & component : object.lock()->components)
			{
				std::weak_ptr<T> cast = std::dynamic_pointer_cast<T>(component.lock());
				if (cast.expired()) continue;

				arrays->emplace_back(cast);
			}

			object.lock()->GetComponentsWithParent<T>(arrays);
		}

		// 子のコンポーネント群を取得
		template <typename T>
		void GetComponentsWithChild(std::list<std::weak_ptr<T>> * arrays)
		{
			for (auto & child : transform.lock()->GetChilds())
			{
				auto obj = child.lock()->gameObject;
				if (obj.expired()) continue;

				for (auto & component : obj.lock()->components)
				{
					std::weak_ptr<T> cast = std::dynamic_pointer_cast<T>(component.lock());
					if (cast.expired()) continue;

					arrays->emplace_back(cast);
				}

				obj.lock()->GetComponentsWithChild<T>(arrays);
			}
		}
		
		// コンポーネントの削除
		template <typename T>
		void RemoveComponent()
		{
			for (auto itr = components.begin(); itr != components.end(); ++itr)
			{
				if (typeid(T) == itr->lock()->GetType())
				{
					Component::RemoveComponent(*itr);
					return;
				}
			}
		}
		void RemoveComponent(std::weak_ptr<Component> component)
		{
			for (auto itr = components.begin(); itr != components.end(); ++itr)
			{
				if (component.lock() == itr->lock())
				{
					Component::RemoveComponent(*itr);
					return;
				}
			}
		}

		// リミット( 秒数 )経過後にオブジェクトを削除する
		static void Destroy(std::weak_ptr<GameObject> gameObject, float limit = 0);

		// シーンを変更しても削除されないオブジェクトとして登録
		static void DontDestroyOnLoad(std::weak_ptr<GameObject> object);

	private:

		static void DontDestroyOnLoadChild(std::weak_ptr<Transform> child);

		// 自身のインスタンスを入れる
		void Initialize(std::shared_ptr<GameObject> thisObject);
		void Finalize();
		void RemoveComponentList(Component * component);
		void SetTransform(std::weak_ptr<Transform> & transform);
		void SetMonoBehaviour(std::weak_ptr<MonoBehaviour> & behaviour);
		void RemoveMonoBehaviour(MonoBehaviour * behaviour);

		//// 当たり判定で当たった時に呼ぶ
		void OnCollision(std::weak_ptr<Collision> mine, std::weak_ptr<Collision> other);
		void OnTrigger(std::weak_ptr<Collision> mine, std::weak_ptr<Collision> other);

		// リミット( 秒数 )経過後にオブジェクトを削除する
		void OnDestroy(float limit = 0);

		// シリアル生成時の関数
		void LoadSerialized();

		std::list<std::weak_ptr<Component>> components;
		std::list<std::weak_ptr<MonoBehaviour>> behaviours;

		Layer layer;
		bool isActive;	// オブジェクトが有効かどうか
		bool isStop;	// オブジェクトを一時停止するためのフラグ
		bool isDontDestroyOnLoad;	// シーンを切り替えた時に消されるかのフラグ
	};
}

#endif // !_GAMEOBJECT_H_
