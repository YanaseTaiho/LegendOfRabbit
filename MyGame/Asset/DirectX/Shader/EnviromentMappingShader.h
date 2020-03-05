#ifndef __ENVIROMENT_MAPPING_SHADER___H___
#define __ENVIROMENT_MAPPING_SHADER___H___

#include "Shader.h"

namespace MyDirectX
{
	class EnviromentMappingShader : public Shader
	{
		friend cereal::access;
		template <typename Archive>
		void save(Archive & archive, std::uint32_t const version) const
		{

		}
		template <typename Archive>
		void load(Archive & archive, std::uint32_t const version)
		{

		}
	public:
		EnviromentMappingShader() {};
		~EnviromentMappingShader() {};

		void Draw(Transform * transform, const Material * material, const MeshData<VTX_MESH> * mesh, unsigned short startIndex, unsigned short indexNum) override;
		void Draw(Transform * transform, const Material * material, const MeshData<VTX_SKIN_MESH> * mesh, unsigned short startIndex, unsigned short indexNum) override;
		void DrawImGui(Material * material, int & id) override;
	};
}

CEREAL_CLASS_VERSION(MyDirectX::EnviromentMappingShader, 0)
CEREAL_REGISTER_TYPE(MyDirectX::EnviromentMappingShader)
CEREAL_REGISTER_POLYMORPHIC_RELATION(MyDirectX::Shader, MyDirectX::EnviromentMappingShader)

#endif // !__ENVIROMENT_MAPPING_SHADER___H___

