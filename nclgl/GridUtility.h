#pragma once

#include "Vector3.h"
#include "Vector4.h"

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

class GridUtility
{
public:

	static CubePlanes GenerateCubePlanes(const Vector3 position, const Vector3 dimensions);
	static Tile GenerateTile(const Vector3 position, const Vector3 dimensions);

private:
	static void AddFaces(CubePlanes& cube, const Vector3 position, const Vector3 dimensions);
	static void AddPositions(CubePlanes& cube, const Vector3 position, const Vector3 dimensions);
};

