#ifndef __SKYDOME_SHADER___H___
#define __SKYDOME_SHADER___H___

#include "Shader.h"

namespace MyDirectX
{
	class SkyDomeShader : public Shader
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
		SkyDomeShader() {};
		~SkyDomeShader() {};

		void Draw(Transform * transform, const Material * material, const MeshData<VTX_MESH> * mesh, unsigned short startIndex, unsigned short indexNum) override;
		//void Draw(const Material * material, const MeshData<VTX_SKIN_MESH> * mesh, unsigned short startIndex, unsigned short indexNum) override;
	};
}

CEREAL_CLASS_VERSION(MyDirectX::SkyDomeShader, 0)
CEREAL_REGISTER_TYPE(MyDirectX::SkyDomeShader)
CEREAL_REGISTER_POLYMORPHIC_RELATION(MyDirectX::Shader, MyDirectX::SkyDomeShader)

#endif // !__SKYDOME_SHADER___H___


