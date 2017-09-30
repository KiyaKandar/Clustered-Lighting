#pragma once

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glew32.lib")

#include "GL/glew.h"
#include "GL/wglew.h"
#include "SOIL.h"

#include "Vector2.h"
#include "Matrix4.h"
#include "Plane.h"
#include "Cube.h"

#include "ComputeShader.h"
#include "GridUtility.h"

#include <vector>
#include <type_traits>

class Light;

const bool THROW_ERROR = true;

struct TileData 
{
	int lightIndexes[1000];
	int tileLights[1000][100];
};

struct ScreenSpaceData
{
	float indexes[100];
	Vector4 numLightsIn;

	Vector4 data[100];
};

class TileRenderer
{
public:
	TileRenderer(Light** lights, int numLights, int numXTiles, int numYTiles, int numZTiles, 
		Vector2 minScreenCoord, Vector2 maxScreenCoord);
	TileRenderer();

	~TileRenderer() 
	{
		delete compute;
	}

	TileData* GetTileData()
	{
		return tileData;
	}

	Tile* GetScreenTiles()
	{
		return screenTiles;
	}

	int GetNumTiles() 
	{
		return numTiles;
	}

	void GenerateGrid();
	void InitGridSSBO();

	void AllocateLightsCPU(const Matrix4& projectionMatrix, const Matrix4& viewMatrix, GLuint buffer, const Vector3& cameraPos);
	void AllocateLightsGPU(const Matrix4& projectionMatrix, const Matrix4& viewMatrix, const Vector3& cameraPos);
	
	ComputeShader* dataPrep;
	ComputeShader* compute;

private:
	void PrepareDataCPU(const Matrix4& projectionMatrix, const Matrix4& viewMatrix, const Vector3& cameraPos);
	void PrepareDataGPU(const Matrix4& projectionMatrix, const Matrix4& viewMatrix, const Vector3& cameraPos);

	void FillTilesGPU();
	void FillTilesCPU(GLuint buffer);
	void CullLights();

	Light** lights;

	Vector2 minCoord;

	int numLights;
	int numTiles;
	int numLightsInFrustum = 0;

	Vector3 gridSize;
	Vector3 gridDimensions;

	//Data
	Tile screenTiles[1000];
	Cube grid[1000];
	CubePlanes* gridPlanes;
	
	Cube screenCube;
	TileData* tileData;

	std::vector<int> lightsInFrustrum;

	Vector4 screenLightData[100];
	ScreenSpaceData ssdata;
	Matrix4 lightModelMatrices[100];

	//SSBO Stuff
	GLuint tileDataSSBO;
	GLuint gridPlanesSSBO;
	GLuint screenSpaceDataSSBO;
	GLuint modelMatricesSSBO;
	GLuint screenCubeSSBO;

	GLuint countBuffer;
	GLuint count;
	CubePlanes screenPlanes;

	//Uniform location
	GLuint loc_numZTiles;
	GLuint loc_projMatrix;
	GLuint loc_projView;
	GLuint loc_cameraPos;

	int inds[100];
};

