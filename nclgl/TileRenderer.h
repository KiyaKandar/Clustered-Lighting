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
	int tileLights[1000][10];
};

struct ScreenSpaceData
{
	Vector4 data[10];
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

	Vector3 gridSize;
	Vector3 gridDimensions;

	//Data
	Tile screenTiles[1000];

	Cube grid[1000];
	CubePlanes* gridPlanes;// [1000];
	
	Cube screenCube;
	TileData* tileData;

	std::vector<int> lightsInFrustrum;

	Vector4 screenLightData[10];
	ScreenSpaceData ssdata;
	Matrix4 lightModelMatrices[10];

	ComputeShader* compute;

	//SSBO Stuff
	GLuint tileDataSSBO;
	GLuint gridPlanesSSBO;
	GLuint screenSpaceDataSSBO;
};

