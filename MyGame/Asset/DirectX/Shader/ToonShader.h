#ifndef _TOON_MESH_SHADER___H___
#define _TOON_MESH_SHADER___H___

#include "Shader.h"

namespace MyDirectX
{
	class ToonShader : public Shader
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
		ToonShader() {};
		~ToonShader() {};

		void Draw(const Material * material, const MeshData<VTX_MESH> * mesh, unsigned short startIndex, unsigned short indexNum) override;
		void Draw(const Material * material, const MeshData<VTX_SKIN_MESH> * mesh, unsigned short startIndex, unsigned short indexNum) override;
		void DrawImGui(Material * material, int & id) override;

		std::weak_ptr<Texture> toonTexture;
	};
}

CEREAL_CLASS_VERSION(MyDirectX::ToonShader, 0)
CEREAL_REGISTER_TYPE(MyDirectX::ToonShader)
CEREAL_REGISTER_POLYMORPHIC_RELATION(MyDirectX::Shader, MyDirectX::ToonShader)

#endif // !_TOON_MESH_SHADER___H___


