#pragma once

#include "Vector2.h"
#include "Matrix4.h"
#include "Plane.h"
#include "Cube.h"

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

struct TileData 
{
	int lightIndexes[1000];
	int tileLights[1000][10];
};

class TileRenderer
{
public:
	TileRenderer(Light** lights, int numLights, int numXTiles, int numYTiles, int numZTiles, 
		Vector2 minScreenCoord, Vector2 maxScreenCoord);
	TileRenderer();

	~TileRenderer() {}

	TileData* GetTileData()
	{
		return &tileData;
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

	void PrepareData(const Matrix4& projectionMatrix, const Matrix4& viewMatrix);
	void CullLights();
	void FillTiles();

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
	Cube screenCube;
	TileData tileData;

	std::vector<int> lightsInFrustrum;

	Vector4 screenLightData[10];
	Matrix4 lightModelMatrices[10];
};

