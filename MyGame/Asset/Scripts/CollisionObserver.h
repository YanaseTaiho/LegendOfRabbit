#ifndef __CollisionObserver__H__
#define __CollisionObserver__H__

#include "FrameWork/Common.h"

// 衝突しているコリジョンの管理をするクラス

class CollisionObserver : public MonoBehaviour
{
private:
	friend cereal::access;
	template<class Archive>
	void save(Archive & archive, std::uint32_t const version) const
	{
		archive(cereal::base_class<MonoBehaviour>(this));

	}

	template<class Archive>
	void load(Archive & archive, std::uint32_t const version)
	{
		archive(cereal::base_class<MonoBehaviour>(this));
	}

	void DrawImGui(int id) override;

public:
	CollisionObserver() {}
	~CollisionObserver() {}

	std::list<std::weak_ptr<Collision>> collisionList;			// 当たっているコリジョンをリストに保持
	std::list<std::weak_ptr<Collision>> collisionTriggerList;	// 当たっているトリガーコリジョンをリストに保持

	void SetCollisionEnter(std::function<void(std::weak_ptr<Collision>)> func) { collisionEnter = func; }
	void SetCollisionExit(std::function<void(std::weak_ptr<Collision>)> func) { collisionExit = func; }
	void SetTriggerEnter(std::function<void(std::weak_ptr<Collision>)> func) { triggerEnter = func; }
	void SetTriggerExit(std::function<void(std::weak_ptr<Collision>)> func) { triggerExit = func; }

private:

	std::list<std::weak_ptr<Collision>> addCollisionList;			// フレーム毎の当たっているコリジョンをリストに保持
	std::list<std::weak_ptr<Collision>> addCollisionTriggerList;	// フレーム毎の当たっているトリガーコリジョンをリストに保持

	// 条件に応じてこの関数を呼ぶ
	std::function<void(std::weak_ptr<Collision>)> collisionEnter;
	std::function<void(std::weak_ptr<Collision>)> collisionExit;
	std::function<void(std::weak_ptr<Collision>)> triggerEnter;
	std::function<void(std::weak_ptr<Collision>)> triggerExit;

	void Update() override;
	void LateUpdate() override;

	// 物理的に当たっている間は呼ばれる
	void OnCollisionStay(std::weak_ptr<Collision> & mine, std::weak_ptr<Collision> & other) override;
	void OnTriggerStay(std::weak_ptr<Collision> & mine, std::weak_ptr<Collision> & other) override;
};

CEREAL_CLASS_VERSION(CollisionObserver, 0)
CEREAL_REGISTER_TYPE(CollisionObserver)

#endif // !__CollisionObserver__H__