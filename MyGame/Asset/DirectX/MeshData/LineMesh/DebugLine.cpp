#include "DebugLine.h"
#include "Main/manager.h"

using namespace MyDirectX;

std::unique_ptr<RayMesh> DebugLine::rayMesh;
std::unordered_map<std::string, std::unique_ptr<LineMesh>> DebugLine::lineMap;

std::vector<DebugLine::DrawRayData> DebugLine::drawRayDataArray;

#define CIRCLE_POINT_NUM (100)
#define GRID_NUM (100)

void DebugLine::RegisterLineData(std::string name, LineMesh * data)
{
#if EDITOR_MODE
	lineMap[name].reset(data);
#endif // EDITOR_MODE
}

bool DebugLine::IsRegisterdLineData(std::string name)
{
#if EDITOR_MODE
	return lineMap.count(name) > 0;
#else
	return false;
#endif // EDITOR_MODE
}

void DebugLine::CreateDebugLines()
{
#if EDITOR_MODE
	rayMesh = std::make_unique<RayMesh>();
	CreateLineGrid();
	CreateLineSphere();
	CreateLineHalfSphere();
	CreateLineCircle();
	CreateLineHalfCircle();
	CreateLineBox();
#endif // EDITOR_MODE
}

void DebugLine::DrawLine(std::string name, Matrix4 matrix, Color color, float width)
{
#if EDITOR_MODE
	if (lineMap.count(name) == 0) return;

	lineMap[name]->Draw(matrix, color, width);
#endif // EDITOR_MODE
}

void DebugLine::DrawRay(Vector3 start, Vector3 end, Color color, float width)
{
#if EDITOR_MODE
	drawRayDataArray.emplace_back(start, end, color, width);
#endif // EDITOR_MODE
}

void DebugLine::DrawRay(Vector3 start, Vector3 dir, float length, Color color, float width)
{
#if EDITOR_MODE
	drawRayDataArray.emplace_back(start, start + dir * length, color, width);
#endif // EDITOR_MODE
}

void DebugLine::DrawRayMoment(Vector3 start, Vector3 dir, float length, Color color, float width)
{
#if EDITOR_MODE
	rayMesh->Draw(start, dir, length, color, width);
#endif // EDITOR_MODE
}

void DebugLine::DrawRayAll()
{
#if EDITOR_MODE
	for (auto data : drawRayDataArray)
	{
		rayMesh->Draw(data.start, data.end, data.color, data.width);
	}
#endif // EDITOR_MODE
}

void DebugLine::DrawDataReset()
{
#if EDITOR_MODE
	std::vector<DrawRayData>().swap(drawRayDataArray);
#endif // EDITOR_MODE
}

void DebugLine::Release()
{
#if EDITOR_MODE
	lineMap.clear();
#endif // EDITOR_MODE
}

void DebugLine::CreateLineGrid()
{
	LineMesh * mesh = new LineMesh();

	const float size = 10.0f;

	const int num = GRID_NUM * 2;
	std::vector<VTX_LINE> vertex;
	vertex.resize(num + 4);

	const float x = (float)GRID_NUM / 4.0f * size;
	const float z = (float)GRID_NUM / 4.0f * size;

	for (int i = 0; i <= GRID_NUM / 2; i++)
	{
		int cnt1 = i * 2;
		int cnt2 = i * 2 + GRID_NUM;
		vertex[cnt1].pos = Vector3(-x + i * size, 0.0f, -z);
		vertex[cnt1 + 1].pos = Vector3(-x + i * size, 0.0f, z);

		vertex[cnt2].pos = Vector3(-x, 0.0f, -z + i * size);
		vertex[cnt2 + 1].pos = Vector3(x, 0.0f, -z + i * size);
	}
	/*VTX_LINE add; 
	add.pos = Vector3(, 0.0f, -z); vertex.emplace_back(add);
	add.pos = Vector3(-x + i, 0.0f, -z); vertex.emplace_back(add);*/

	mesh->meshData.CreateVertexBuffer(&vertex);

	std::vector<VTX_LINE>().swap(vertex);

	RegisterLineData("Grid", mesh);
}

void DebugLine::CreateLineSphere()
{
	LineMesh * mesh = new LineMesh();

	const int num = ((CIRCLE_POINT_NUM % 2) == 0) ? CIRCLE_POINT_NUM : CIRCLE_POINT_NUM + 1;
	std::vector<VTX_LINE> vertex;
	vertex.resize(num * 3);

	float rad = 0.0f;
	const float add = 360.0f / ((float)num * 0.5f);
	const int offset1 = num;
	const int offset2 = num * 2;

	for (int i = 0; i < num; i++)
	{
		float x = cosf(Mathf::DegToRad(rad));
		float z = sinf(Mathf::DegToRad(rad));

		vertex[i].pos = XMFLOAT3(x, 0.0f, z);
		vertex[i + offset1].pos = XMFLOAT3(x, z, 0.0f);
		vertex[i + offset2].pos = XMFLOAT3(0.0f, z, x);

		if(i % 2 == 0) rad += add;
	}

	mesh->meshData.CreateVertexBuffer(&vertex);

	std::vector<VTX_LINE>().swap(vertex);

	RegisterLineData("Sphere", mesh);
}

void DebugLine::CreateLineHalfSphere()
{
	LineMesh * mesh = new LineMesh();

	const int num = ((CIRCLE_POINT_NUM % 2) == 0) ? CIRCLE_POINT_NUM : CIRCLE_POINT_NUM + 1;
	std::vector<VTX_LINE> vertex;
	vertex.resize(num * 3);

	float radius1 = 0.0f;
	float radius2 = 0.0f;
	const float add1 = 360.0f / ((float)num * 0.5f);
	const float add2 = 180.0f / ((float)num * 0.5f);
	const int offset1 = num;
	const int offset2 = num * 2;

	for (int i = 0; i < num; i++)
	{
		float x1 = cosf(Mathf::DegToRad(radius1));
		float z1 = sinf(Mathf::DegToRad(radius1));
		float x2 = cosf(Mathf::DegToRad(radius2));
		float z2 = sinf(Mathf::DegToRad(radius2));

		vertex[i].pos = XMFLOAT3(x1, 0.0f, z1);
		vertex[i + offset1].pos = XMFLOAT3(x2, z2, 0.0f);
		vertex[i + offset2].pos = XMFLOAT3(0.0f, z2, x2);

		if (i % 2 == 0)
		{
			radius1 += add1;
			radius2 += add2;
		}
	}

	mesh->meshData.CreateVertexBuffer(&vertex);

	RegisterLineData("HalfSphere", mesh);
}

void DebugLine::CreateLineCircle()
{
	LineMesh * mesh = new LineMesh();

	const int num = ((CIRCLE_POINT_NUM % 2) == 0) ? CIRCLE_POINT_NUM : CIRCLE_POINT_NUM + 1;
	std::vector<VTX_LINE> vertex;
	vertex.resize(num);

	float radius = 0.0f;
	const float add = 360.0f / ((float)num * 0.5f);

	for (int i = 0; i < num; i++)
	{
		float x1 = cosf(Mathf::DegToRad(radius));
		float z1 = sinf(Mathf::DegToRad(radius));

		vertex[i].pos = XMFLOAT3(x1, z1, 0.0f);

		if (i % 2 == 0)
		{
			radius += add;
		}
	}

	mesh->meshData.CreateVertexBuffer(&vertex);

	RegisterLineData("Circle", mesh);
}

void DebugLine::CreateLineHalfCircle()
{
	LineMesh * mesh = new LineMesh();

	const int num = ((CIRCLE_POINT_NUM % 2) == 0) ? CIRCLE_POINT_NUM : CIRCLE_POINT_NUM + 1;
	std::vector<VTX_LINE> vertex;
	vertex.resize(num);

	float radius = 0.0f;
	const float add = 180.0f / ((float)num * 0.5f);

	for (int i = 0; i < num; i++)
	{
		float x1 = cosf(Mathf::DegToRad(radius));
		float z1 = sinf(Mathf::DegToRad(radius));

		vertex[i].pos = XMFLOAT3(x1, z1, 0.0f);

		if (i % 2 == 0)
		{
			radius += add;
		}
	}

	mesh->meshData.CreateVertexBuffer(&vertex);

	RegisterLineData("HalfCircle", mesh);
}

void DebugLine::CreateLineBox()
{
}
