#ifndef __FAR_SHADER___H___
#define __FAR_SHADER___H___

#include "Shader.h"
#include "../Texture/TextureManager.h"

namespace MyDirectX
{
	class FarShader : public Shader
	{
		friend cereal::access;
		template <typename Archive>
		void save(Archive & archive, std::uint32_t const version) const
		{
			std::string texName;
			if ((!farTexture.expired())) texName = farTexture.lock()->GetName();
			archive(texName, instanceCount, scaleOffset);
		}
		template <typename Archive>
		void load(Archive & archive, std::uint32_t const version)
		{
			std::string texName;
			archive(texName, instanceCount, scaleOffset);
			farTexture = Singleton<TextureManager>::Instance()->Load(texName);
		}
	public:
		FarShader() {};
		~FarShader() {};

		void Draw(Transform * transform, const Material * material, const MeshData<VTX_MESH> * mesh, unsigned short startIndex, unsigned short indexNum) override;
		void Draw(Transform * transform, const Material * material, const MeshData<VTX_SKIN_MESH> * mesh, unsigned short startIndex, unsigned short indexNum) override;
		void DrawImGui(Material * material, int & id) override;

		std::weak_ptr<Texture> farTexture;
		UINT instanceCount = 10;		// ï`âÊâÒêî
		float scaleOffset = 0.01f;  // ñ@ê¸ï˚å¸Ç…ägëÂÇµÇƒÇ¢Ç≠ä‘äu
	};
}

CEREAL_CLASS_VERSION(MyDirectX::FarShader, 0)
CEREAL_REGISTER_TYPE(MyDirectX::FarShader)
CEREAL_REGISTER_POLYMORPHIC_RELATION(MyDirectX::Shader, MyDirectX::FarShader)

#endif // !__FAR_SHADER___H___

