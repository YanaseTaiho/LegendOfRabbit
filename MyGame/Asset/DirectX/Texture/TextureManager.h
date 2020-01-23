#ifndef _TEXTUREMANAGER_H_
#define _TEXTUREMANAGER_H_

#include "Texture.h"
#include "../../FrameWork/Singleton.h"
#include <unordered_map>
#include <memory>

namespace MyDirectX
{
	class TextureManager
	{
		friend FrameWork::Singleton<TextureManager>;

		TextureManager();
		~TextureManager();

	public:

		void SaveTexture(std::string path);
		void LoadTexture(std::string path);

		// テクスチャをロードする
		// 既にロードしていた場合はロードせずにそのテクスチャを返す
		std::weak_ptr<Texture> Load(std::string fileName);
		std::weak_ptr<Texture> GetTexture(std::string fileName);

		void DrawImGui();
		void BeginDragImGuiTexture(std::weak_ptr<Texture> texture, const Vector2 & size, std::string id);
		bool DropTargetImGuiTexture(std::weak_ptr<Texture> & texture, const Vector2 & size, std::string id);

	private:
	 	std::unordered_map<std::string, std::shared_ptr<Texture>> textureMap;
	};
}

#endif //!_TEXTUREMANAGER_H_