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

#include <vector>
#include <type_traits>

const bool THROW_ERROR = true;

class Light;

struct Tile
{
	//Coord is bottom left of tile
	float xCoord;
	float yCoord;
	float zCoord;
	float width;
	float height;
	float length;

	float padding[6];
};

struct CubePlanes
{
	Vector4 faces[6];
	Vector4 positions[6];
};

struct TileData 
{
	int lightIndexes[1000];
	int tileLights[1000][100];
};

struct ScreenSpaceData
{
	float indexes[100];
	//float padding[9];
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

	void PrepareData(const Matrix4& projectionMatrix, const Matrix4& viewMatrix);
	void CullLights();

	void FillTilesGPU();
	void FillTilesCPU(GLuint buffer);

private:
	Tile GenerateTile(Vector3 position, Vector3 dimensions) const;

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
	CubePlanes* gridPlanes;// [1000];
	
	Cube screenCube;
	TileData* tileData;

	std::vector<int> lightsInFrustrum;

	Vector4 screenLightData[100];
	ScreenSpaceData ssdata;
	Matrix4 lightModelMatrices[100];

	ComputeShader* compute;
	ComputeShader* dataPrep;

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
	GLint loc_numZTiles;
};

