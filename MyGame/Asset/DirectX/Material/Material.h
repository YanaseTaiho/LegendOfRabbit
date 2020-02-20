#ifndef __MATERIAL_H__
#define __MATERIAL_H__

#include "FrameWork/Color.h"
#include "../Texture/Texture.h"
#include "../Shader/ConstantBuffer.h"
#include "../Shader/Shader.h"

using namespace FrameWork;

namespace MyDirectX
{
	enum class MaterialType : int
	{
		Soil,  // 土
		Wood,  // 木
		Stone, // 石
		Iron,  // 鉄
		MaxNum
	};

	static std::string enum_to_string(MaterialType type)
	{
		switch (type)
		{
		case MaterialType::Soil: return "Soil";
		case MaterialType::Wood: return "Wood";
		case MaterialType::Stone: return "Stone";
		case MaterialType::Iron: return "Iron";
		}
		return "";
	}

	class Material
	{
	private:
		friend cereal::access;
		template <typename Archive>
		void save(Archive & archive, std::uint32_t const version) const
		{
			std::string texName, normalName, heightName;
			if ((!pTexture.expired())) texName = pTexture.lock()->GetName();
			if ((!pNormalTexture.expired())) normalName = pNormalTexture.lock()->GetName();
			if ((!pHeightTexture.expired())) heightName = pHeightTexture.lock()->GetName();

			archive(CEREAL_NVP(name), CEREAL_NVP(materialParam), CEREAL_NVP(shader),
				CEREAL_NVP(texName),
				CEREAL_NVP(normalName),
				CEREAL_NVP(heightName));

			if (version >= 1) archive(rasterizer);
			if (version >= 2) archive(type);
		}
		template <typename Archive>
		void load(Archive & archive, std::uint32_t const version)
		{
			std::string texName, normalName, heightName;
			archive(CEREAL_NVP(name), CEREAL_NVP(materialParam), CEREAL_NVP(shader),
				CEREAL_NVP(texName),
				CEREAL_NVP(normalName),
				CEREAL_NVP(heightName));

			if (version >= 1) archive(rasterizer);
			if (version >= 2) archive(type);

			LoadSerialize(texName, normalName, heightName);
		}
		void LoadSerialize(std::string tex, std::string nor, std::string hei);

	public:
		Material();
		~Material();

		void SetShader(Shader * shader);

		void DrawImGui(int & id);

		// テクスチャをピクセルシェーダーにセット
		void SetPSResources() const;
		void SetOption() const;

		void SetTexture(std::string fileName);
		void SetNormalTexture(std::string fileName);
		void SetHeightTexture(std::string fileName);

		std::shared_ptr<Shader> shader;
		Rasterizer rasterizer;
		std::string name;
		CB_MATERIAL materialParam;
		std::weak_ptr<Texture> pTexture;
		std::weak_ptr<Texture> pNormalTexture;
		std::weak_ptr<Texture> pHeightTexture;

		MaterialType type;
	};
}

CEREAL_CLASS_VERSION(MyDirectX::Material, 2)

#endif //!__MATERIAL_H__
