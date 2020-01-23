#ifndef _GEOMETRYSHDAERDATA_H_
#define _GEOMETRYSHDAERDATA_H_

#include "../../Renderer/System/RendererSystem.h"
#include <string>

namespace MyDirectX
{
	class GeometryShaderInfo
	{
	public:
		GeometryShaderInfo();
		~GeometryShaderInfo();

		void CreateShader(const std::wstring gemPath);

		ID3D11GeometryShader *m_pGeomtryShader;

	private:


	};
}

#endif // !_GEOMETRYSHDAERDATA_H_