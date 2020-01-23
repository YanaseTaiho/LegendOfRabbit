#ifndef __MESH_SHADER___H___
#define __MESH_SHADER___H___

#include "Shader.h"

namespace MyDirectX
{
	class MeshShader : public Shader
	{
		friend cereal::access;
		template <typename Archive>
		void serialize(Archive & archive)
		{
		}
	public:
		MeshShader() {};
		~MeshShader() {};

		void SetOption(const Material * material) override;
		void DrawImGui(Material * material, int & id) override;
	};
}

CEREAL_REGISTER_TYPE(MyDirectX::MeshShader)
CEREAL_REGISTER_POLYMORPHIC_RELATION(MyDirectX::Shader, MyDirectX::MeshShader)

#endif // !__MESH_SHADER___H___

