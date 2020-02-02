#ifndef __ENVIROMENT_MAPPING_SHADER___H___
#define __ENVIROMENT_MAPPING_SHADER___H___

#include "Shader.h"

namespace MyDirectX
{
	class EnviromentMappingShader : public Shader
	{
		friend cereal::access;
		template <typename Archive>
		void serialize(Archive & archive)
		{
		}
	public:
		EnviromentMappingShader() {};
		~EnviromentMappingShader() {};

		void SetOption(const Material * material) override;
		void DrawImGui(Material * material, int & id) override;
	};
}

CEREAL_REGISTER_TYPE(MyDirectX::EnviromentMappingShader)
CEREAL_REGISTER_POLYMORPHIC_RELATION(MyDirectX::Shader, MyDirectX::EnviromentMappingShader)

#endif // !__ENVIROMENT_MAPPING_SHADER___H___

