#ifndef _COMMON_H_
#define _COMMON_H_

#include <string>
#define FilePathTexture(name) (std::string("Asset/Art/Texture/") + std::string(name)).data()
#define FilePathTextureEx(name) (std::string("Asset\\Art\\Texture\\") + std::string(name)).data()
#define FilePathModel(name) (std::string("Asset/Art/Model/") + std::string(name)).data()
// シェーダーコンパイル時は「Unicord」
#define FilePathShader(name) (std::wstring(L"Asset/Shader/") + std::wstring(name)).data()

#endif // !_COMMON_H_

