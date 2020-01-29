#ifndef _TOON_MESH_SHADER___H___
#define _TOON_MESH_SHADER___H___

#include "Shader.h"

namespace MyDirectX
{
	class ToonMeshShader : public Shader
	{
		friend cereal::access;
		template <typename Archive>
		void serialize(Archive & archive)
		{
		}
	public:
		ToonMeshShader() {};
		~ToonMeshShader() {};

		void SetOption(const Material * material) override;
		void DrawImGui(Material * material, int & id) override;

		std::weak_ptr<Texture> toonTexture;
	};
}

CEREAL_REGISTER_TYPE(MyDirectX::ToonMeshShader)
CEREAL_REGISTER_POLYMORPHIC_RELATION(MyDirectX::Shader, MyDirectX::ToonMeshShader)

#endif // !_TOON_MESH_SHADER___H___


