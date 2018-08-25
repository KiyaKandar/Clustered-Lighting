#include "GridUtility.h"

#include "../ClusterMaths/Cube.h"
#include "../Game/GraphicsConfiguration/GLConfig.h"

enum
{
	LEFT,
	RIGHT,
	FRONT,
	BACK,
	TOP,
	BOTTOM
};

void GridUtility::Generate3DGrid(GridData gridData, Vector3 dimensions, Vector3 tilesOnAxes)
{
	float xOffset = 0;
	float yOffset = 0;
	const int numTiles = tilesOnAxes.x * tilesOnAxes.y * tilesOnAxes.z;

	int xIndex = 0;
	int yIndex = 0;
	int zIndex = 0;

	for (int i = 0; i < tilesOnAxes.x * tilesOnAxes.y; i++)
	{
		zIndex = 0;
		xIndex = ceilf(xOffset);

		//Once reached the end of x axis, reset x offset and move up y axis.
		if (xIndex == tilesOnAxes.x)
		{
			yOffset += dimensions.y;
			++yIndex;
			xOffset = 0;
			xIndex = 0;
		}

		//Create tile closest to screen.
		const Vector3 startPosition((dimensions.x * xOffset) + gridData.minCoord.x, yOffset + gridData.minCoord.y, GLConfig::NEAR_PLANE);
		int index = xIndex + int(tilesOnAxes.x) * (yIndex + int(tilesOnAxes.y) * zIndex);

		gridData.grid[index] = Cube(startPosition, dimensions);
		gridData.gridPlanes[index] = GenerateCubePlanes(startPosition, dimensions);
		gridData.screenTiles[index] = GenerateTile(startPosition, dimensions);

		//Fill along the z axis from the tile above.
		for (int k = 1; k <= tilesOnAxes.z - 1; ++k)
		{
			zIndex = k;
			const float newZCoord = (dimensions.z * k);
			index = xIndex + int(tilesOnAxes.x) * (yIndex + int(tilesOnAxes.y) * zIndex);

			const Vector3 positionExtendedInZAxis(startPosition.x, startPosition.y, startPosition.z + newZCoord);

			gridData.grid[index] = Cube(positionExtendedInZAxis, dimensions);
			gridData.screenTiles[index] = GenerateTile(positionExtendedInZAxis, dimensions);
			gridData.gridPlanes[index] = GenerateCubePlanes(positionExtendedInZAxis, dimensions);
		}

		++xOffset;
	}
}

CubePlanes GridUtility::GenerateCubePlanes(const Vector3 position, const Vector3 dimensions)
{
	CubePlanes cube;
	AddFaces(cube, position, dimensions);
	AddPositions(cube, position, dimensions);

	return cube;
}

Tile GridUtility::GenerateTile(const Vector3 position, const Vector3 dimensions)
{
	Tile tile;

	tile.xCoord = position.x;
	tile.yCoord = position.y;
	tile.zCoord = position.z;

	tile.width = dimensions.x;
	tile.height = dimensions.y;
	tile.length = dimensions.z;

	return tile;
}

void GridUtility::AddFaces(CubePlanes& cube, const Vector3 position, const Vector3 dimensions)
{
	cube.faces[LEFT] = Vector4(LEFT_NORMAL.x, LEFT_NORMAL.y, LEFT_NORMAL.z, position.Length());
	cube.faces[RIGHT] = Vector4(RIGHT_NORMAL.x, RIGHT_NORMAL.y, RIGHT_NORMAL.z, (position + Vector3(dimensions.x, 0, 0)).Length());
	cube.faces[FRONT] = Vector4(FRONT_NORMAL.x, FRONT_NORMAL.y, FRONT_NORMAL.z, (position + Vector3(0, 0, dimensions.z)).Length());
	cube.faces[BACK] = Vector4(BACK_NORMAL.x, BACK_NORMAL.y, BACK_NORMAL.z, position.Length());
	cube.faces[TOP] = Vector4(TOP_NORMAL.x, TOP_NORMAL.y, TOP_NORMAL.z, (position + Vector3(0, dimensions.y, 0)).Length());
	cube.faces[BOTTOM] = Vector4(BOTTOM_NORMAL.x, BOTTOM_NORMAL.y, BOTTOM_NORMAL.z, position.Length());
}

void GridUtility::AddPositions(CubePlanes& cube, const Vector3 position, const Vector3 dimensions)
{
	cube.positions[LEFT] = Vector4(position.x, position.y, position.z, 0);
	cube.positions[RIGHT] = Vector4(position.x + dimensions.x, position.y, position.z, 0);
	cube.positions[FRONT] = Vector4(position.x, position.y, position.z + dimensions.z, 0);
	cube.positions[BACK] = Vector4(position.x, position.y, position.z, 0);
	cube.positions[TOP] = Vector4(position.x, position.y + dimensions.y, position.z, 0);
	cube.positions[BOTTOM] = Vector4(position.x, position.y, position.z, 0);
}
