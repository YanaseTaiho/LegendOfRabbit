#ifndef __TargetImageController__H__
#define __TargetImageController__H__

#include "FrameWork/Common.h"
#include "../DirectX/Common.h"

class TargetImageController : public MonoBehaviour
{
private:
	friend cereal::access;
	template<class Archive>
	void save(Archive & archive, std::uint32_t const version) const
	{
		archive(cereal::base_class<MonoBehaviour>(this));
		archive(targetScale, rotationSpeed);
	}

	template<class Archive>
	void load(Archive & archive, std::uint32_t const version)
	{
		archive(cereal::base_class<MonoBehaviour>(this));
		archive(targetScale, rotationSpeed);
	}

	void DrawImGui(int id) override;

public:
	TargetImageController() {};
	~TargetImageController() {};

	void TargetStart();

private:
	void Start() override;
	//void Update() override;
	void LateUpdate() override;

	std::weak_ptr<BillboardRenderer> renderer;
	float targetScale;
	float rotationSpeed;
};

CEREAL_CLASS_VERSION(TargetImageController, 0)
CEREAL_REGISTER_TYPE(TargetImageController)

#endif // !__TargetImageController__H__