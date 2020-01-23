#ifndef __SKIN_MESH_SHADER___H___
#define __SKIN_MESH_SHADER___H___

#include "Shader.h"

namespace MyDirectX
{
	class SkinMeshShader : public Shader
	{
		friend cereal::access;
		template <typename Archive>
		void serialize(Archive & archive)
		{
		}
	public:
		SkinMeshShader() {};
		~SkinMeshShader() {};

		void SetOption(const Material * material) override;
		void DrawImGui(Material * material, int & id) override;
	};
}

CEREAL_REGISTER_TYPE(MyDirectX::SkinMeshShader)
CEREAL_REGISTER_POLYMORPHIC_RELATION(MyDirectX::Shader, MyDirectX::SkinMeshShader)

#endif // !__SKIN_MESH_SHADER___H___

