#ifndef _ROTATION_FIXED_CONTROLLER_H_
#define _ROTATION_FIXED_CONTROLLER_H_

#include "FrameWork/Common.h"

class RotationFixedController : public MonoBehaviour
{
private:
	friend cereal::access;
	template<class Archive>
	void save(Archive & archive, std::uint32_t const version) const
	{
		archive(cereal::make_nvp("MonoBehaviour", cereal::base_class<MonoBehaviour>(this)));
		archive(fixedRotationArray);
	}

	template<class Archive>
	void load(Archive & archive, std::uint32_t const version)
	{
		archive(cereal::make_nvp("MonoBehaviour", cereal::base_class<MonoBehaviour>(this)));
		archive(fixedRotationArray);
	}

	void DrawImGui(int id) override;
public:
	RotationFixedController() {};
	~RotationFixedController() {};

	void SetWeight(float speed) { weightSpeed = speed; }
	bool IsWeightDone() { return weightValue >= 1.0f; };
	void LateUpdate() override;

private:

	struct FixedRotation
	{
		std::weak_ptr<Transform> transform;
		Quaternion rotation;

		template<class Archive>
		void serialize(Archive & archive)
		{
			archive(transform, rotation);
		}
	};

	std::vector<FixedRotation> fixedRotationArray;
	float weightSpeed = 0.0f;
	float weightValue = 0.0f;
};

CEREAL_CLASS_VERSION(RotationFixedController, 0)
CEREAL_REGISTER_TYPE(RotationFixedController)

#endif // !_BULLET_H_
