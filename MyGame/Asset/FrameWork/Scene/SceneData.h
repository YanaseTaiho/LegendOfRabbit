#ifndef __SCENE__DATA__H__
#define __SCENE__DATA__H__

//#include "../Component/Transform/NodeTranform.h"
//#include "../GameObject/GameObject.h"
#include "Cereal/Common.h"

namespace FrameWork
{
	class GameObject;

	class SceneData
	{
	public:
		std::string name;
		std::list<std::weak_ptr<GameObject>> gameObjectList;
		//std::list<std::weak_ptr<NodeTransform>> nodeTransformList;
	private:
		friend cereal::access;
		template <typename Archive>
		void save(Archive & archive, std::uint32_t const version) const
		{
			archive(CEREAL_NVP(name), CEREAL_NVP(gameObjectList));
		}
		template <typename Archive>
		void load(Archive & archive, std::uint32_t const version)
		{
			archive(CEREAL_NVP(name), CEREAL_NVP(gameObjectList));
		}
	};
}

CEREAL_CLASS_VERSION(FrameWork::SceneData, 0)

#endif // !_SCENE_DATA_H_

