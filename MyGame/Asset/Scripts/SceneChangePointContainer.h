#ifndef __EntryPointContainer__H__
#define __EntryPointContainer__H__

#include "FrameWork/Common.h"
#include "SceneChangePoint.h"

class SceneChangePointContainer : public MonoBehaviour
{
private:
	friend cereal::access;
	template<class Archive>
	void save(Archive & archive, std::uint32_t const version) const
	{
		archive(cereal::base_class<MonoBehaviour>(this));
		archive(pointArray);
	}

	template<class Archive>
	void load(Archive & archive, std::uint32_t const version)
	{
		archive(cereal::base_class<MonoBehaviour>(this));
		archive(pointArray);
	}

	void DrawImGui(int id) override;

public:

	std::vector<std::weak_ptr<SceneChangePoint>> pointArray;
private:


};

CEREAL_CLASS_VERSION(SceneChangePointContainer, 0)
CEREAL_REGISTER_TYPE(SceneChangePointContainer)

#endif // !__EntryPointContainer__H__