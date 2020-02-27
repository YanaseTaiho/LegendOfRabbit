#ifndef __SKIN_MESH_SHADER___H___
#define __SKIN_MESH_SHADER___H___

#include "Shader.h"

namespace MyDirectX
{
	class StandardShader : public Shader
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
		StandardShader() {};
		~StandardShader() {};

		void Draw(const Material * material, const MeshData<VTX_MESH> * mesh, unsigned short startIndex, unsigned short indexNum) override;
		void Draw(const Material * material, const MeshData<VTX_SKIN_MESH> * mesh, unsigned short startIndex, unsigned short indexNum) override;
		void DrawImGui(Material * material, int & id) override;
	};
}

CEREAL_CLASS_VERSION(MyDirectX::StandardShader, 0)
CEREAL_REGISTER_TYPE(MyDirectX::StandardShader)
CEREAL_REGISTER_POLYMORPHIC_RELATION(MyDirectX::Shader, MyDirectX::StandardShader)

#endif // !__SKIN_MESH_SHADER___H___

