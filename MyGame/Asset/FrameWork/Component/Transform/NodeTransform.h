#ifndef _NODE_TRANSFORM_H_
#define _NODE_TRANSFORM_H_

#include "Transform.h"

namespace FrameWork
{
	class SceneData;

	struct NodeTransform
	{
		static const char *NODE_TRANSFORM_LIST;

		bool isSelect = false;
		bool isTreeNodeOpen = false;
		std::weak_ptr<SceneData> sceneData;
		std::weak_ptr<Transform> transform;
		std::weak_ptr<NodeTransform> nodeParent;
		std::list<std::weak_ptr<NodeTransform>> nodeChilds;

	private:
		friend cereal::access;
		template <typename Archive>
		void serialize(Archive & archive, std::uint32_t const version)
		{
			archive(CEREAL_NVP(sceneData), CEREAL_NVP(transform), CEREAL_NVP(nodeParent), CEREAL_NVP(nodeChilds));
		}
	};
}

CEREAL_CLASS_VERSION(FrameWork::NodeTransform, 0)

#endif //!_NODE_TRANSFORM_H_