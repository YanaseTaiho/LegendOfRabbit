#ifndef _MESHNODE_H_
#define _MESHNODE_H_

#include <memory>
#include "MeshData.h"
#include "../Material/Material.h"

namespace MyDirectX
{
	// マテリアル毎にメッシュ情報を格納するための構造体
	struct Subset
	{
		Material material;
		unsigned short startIndex;
		unsigned short indexNum;

	private:
		friend cereal::access;
		template <typename Archive>
		void serialize(Archive & archive)
		{
			archive(CEREAL_NVP(material), CEREAL_NVP(startIndex), CEREAL_NVP(indexNum));
		}
	};

	template<typename VTX>
	struct MeshNode
	{
		MeshNode() {}
		~MeshNode()
		{
			std::vector<Subset>().swap(subsetArray);
		}

		MeshData<VTX> meshData;
		std::vector<Subset> subsetArray;

	private:
		friend cereal::access;
		template <typename Archive>
		void serialize(Archive & archive)
		{
			archive(CEREAL_NVP(meshData), CEREAL_NVP(subsetArray));
		}
	};
}

#endif // !_MESHNODE_H_
