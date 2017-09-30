#pragma once

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Cube.h"

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

struct GridData
{
	Cube* grid;
	CubePlanes* gridPlanes;
	Tile* screenTiles;
	Vector2 minCoord;

	GridData(Cube* grid, CubePlanes* gridPlanes, Tile* screenTiles, Vector2 minCoord)
	{
		this->grid = grid;
		this->gridPlanes = gridPlanes;
		this->screenTiles = screenTiles;
		this->minCoord = minCoord;
	}
};

class GridUtility
{
public:

	static void Generate3DGrid(GridData gridData, Vector3 dimensions, Vector3 tilesOnAxes);

	static CubePlanes GenerateCubePlanes(const Vector3 position, const Vector3 dimensions);
	static Tile GenerateTile(const Vector3 position, const Vector3 dimensions);

private:
	static void AddFaces(CubePlanes& cube, const Vector3 position, const Vector3 dimensions);
	static void AddPositions(CubePlanes& cube, const Vector3 position, const Vector3 dimensions);
};

