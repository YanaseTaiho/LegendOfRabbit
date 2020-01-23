#ifndef _LINEMANAGER_H_
#define _LINEMANAGER_H_

#include "LineMesh.h"
#include "RayMesh.h"
#include <unordered_map>
#include <vector>

namespace MyDirectX
{
	class DebugLine
	{
	public:

		static void RegisterLineData(std::string name, LineMesh * data);
		static bool IsRegisterdLineData(std::string name);
		static void CreateDebugLines();
		static void DrawLine(std::string name, Matrix4 matrix, Color color, float width = 0.0f);
		static void DrawRay(Vector3 start, Vector3 end, Color color, float width = 0.0f);
		static void DrawRay(Vector3 start, Vector3 dir, float length, Color color, float width = 0.0f);
		static void DrawRayMoment(Vector3 start, Vector3 dir, float length, Color color, float width = 0.0f);
		static void DrawRayAll();
		static void DrawDataReset();
		static void Release();

	private:
		static void CreateLineGrid();
		static void CreateLineSphere();
		static void CreateLineHalfSphere();
		static void CreateLineCircle();
		static void CreateLineHalfCircle();
		static void CreateLineBox();

		static std::unique_ptr<RayMesh> rayMesh;
		static std::unordered_map<std::string, std::unique_ptr<LineMesh>> lineMap;

		struct DrawRayData
		{
			DrawRayData(Vector3 start, Vector3 end, Color color, float width) : start(start), end(end), color(color), width(width) {}
			Vector3 start, end;
			Color color;
			float width;
		};
		static std::vector<DrawRayData> drawRayDataArray;
	};
}

#endif // !_LINEMANAGER_H_