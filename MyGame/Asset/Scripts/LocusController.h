#ifndef __LocusController__H__
#define __LocusController__H__

#include "FrameWork/Common.h"
#include "../DirectX/Common.h"

class LocusController : public MonoBehaviour
{
private:
	friend cereal::access;
	template<class Archive>
	void save(Archive & archive, std::uint32_t const version) const
	{
		archive(cereal::base_class<MonoBehaviour>(this));
		archive(locusRenderer, locusTransform1, locusTransform2);
		archive(frameMax, remainTime);
	}

	template<class Archive>
	void load(Archive & archive, std::uint32_t const version)
	{
		archive(cereal::base_class<MonoBehaviour>(this));
		archive(locusRenderer, locusTransform1, locusTransform2);
		archive(frameMax, remainTime);

		framePos1.resize(frameMax);
		framePos2.resize(frameMax);
	}

	void DrawImGui(int id) override;

public:
	LocusController() {};
	~LocusController();

	std::weak_ptr<LocusRenderer> locusRenderer;
	std::weak_ptr<Transform> locusTransform1;
	std::weak_ptr<Transform> locusTransform2;
	std::vector<Vector3> framePos1;
	std::vector<Vector3> framePos2;
	int startFrameCnt = 0;	// 軌跡の先頭のフレーム
	int endFrameCnt = 0;	// 軌跡の末端のフレーム
	int frameMax = 0;	// 軌跡の個数の最大
	int remainTime = 0;	// 軌跡の残り続ける時間
	int remainCnt = 0;
	bool isStart = false;

	void SetCollision(std::weak_ptr<GameObject> myObject, std::function<void(MeshCastInfo)> hitFunc);

	void LocusStart();

private:

	std::weak_ptr<GameObject> myObject;
	std::function<void(MeshCastInfo hitInfo)> CollisionFunc = nullptr;	// これをセットすると当たり判定を行う

	//void Start() override;
	//void Update() override;
	void LateUpdate() override;
};

CEREAL_CLASS_VERSION(LocusController, 0)
CEREAL_REGISTER_TYPE(LocusController)

#endif // !__LocusController__H__