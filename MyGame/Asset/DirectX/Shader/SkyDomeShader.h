#ifndef __SKYDOME_SHADER___H___
#define __SKYDOME_SHADER___H___

#include "Shader.h"

namespace MyDirectX
{
	class SkyDomeShader : public Shader
	{
		friend cereal::access;
		template <typename Archive>
		void serialize(Archive & archive)
		{
		}
	public:
		SkyDomeShader() {};
		~SkyDomeShader() {};

		void SetOption(const Material * material) override;
	};
}

CEREAL_REGISTER_TYPE(MyDirectX::SkyDomeShader)
CEREAL_REGISTER_POLYMORPHIC_RELATION(MyDirectX::Shader, MyDirectX::SkyDomeShader)

#endif // !__SKYDOME_SHADER___H___


