#ifndef __Template_MonoBehaviour__H__
#define __Template_MonoBehaviour__H__

#include "FrameWork/Common.h"

class Template_MonoBehaviour : public MonoBehaviour
{
private:
	friend cereal::access;
	template<class Archive>
	void save(Archive & archive, std::uint32_t const version) const
	{
		archive(cereal::make_nvp("MonoBehaviour", cereal::base_class<MonoBehaviour>(this)));

	}

	template<class Archive>
	void load(Archive & archive, std::uint32_t const version)
	{
		archive(cereal::make_nvp("MonoBehaviour", cereal::base_class<MonoBehaviour>(this)));

	}

	void DrawImGui(int id) override;

public:
	Template_MonoBehaviour();
	~Template_MonoBehaviour();

private:
	void Start() override;
	void Update() override;
	void LateUpdate() override;

	// デバッグ用
	void Draw() override;

	// 物理的に当たっている間は呼ばれる
	void OnCollisionStay(std::weak_ptr<Collision> & mine, std::weak_ptr<Collision> & other) override;
	void OnTriggerStay(std::weak_ptr<Collision> & mine, std::weak_ptr<Collision> & other) override;
};

CEREAL_CLASS_VERSION(Template_MonoBehaviour, 0)
CEREAL_REGISTER_TYPE(Template_MonoBehaviour)

#endif // !__Template_MonoBehaviour__H__