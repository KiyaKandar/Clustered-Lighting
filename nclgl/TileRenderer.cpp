#include "TileRenderer.h"

#include "Light.h"
#include "../Game/Util.h"

const int INTERSECTING	= 1;
const int EMPTY			= 0;

TileRenderer::TileRenderer(Light** lights, int numLights, int numXTiles, int numYTiles, int numZTiles,
	Vector2 minScreenCoord, Vector2 maxScreenCoord)
{
	this->numLights	= numLights;
	this->minCoord	= minScreenCoord;
	this->lights	= lights;

	//Check number of tiles is greater than 0
	Util::AssertGreaterThan<int>(numXTiles, 0, false, THROW_ERROR);
	Util::AssertGreaterThan<int>(numYTiles, 0, false, THROW_ERROR);
	Util::AssertGreaterThan<int>(numZTiles, 0, false, THROW_ERROR);

	gridSize		= Vector3(numXTiles, numYTiles, numZTiles);
	gridDimensions	= Vector3(
		std::abs(minScreenCoord.x - maxScreenCoord.x) / (float)gridSize.x,
		std::abs(minScreenCoord.y - maxScreenCoord.y) / (float)gridSize.y,
		1 / (float)gridSize.z);

	numTiles = gridSize.x * gridSize.y * gridSize.z;

	for (int i = 0; i < numLights; ++i)
	{
		Matrix4 modelMat;
		modelMat.SetPositionVector(lights[i]->GetPosition());

		lightModelMatrices[i] = modelMat;
	}
}

TileRenderer::TileRenderer()
{
	numLights = 0;

	minCoord		= Vector2();
	gridSize		= Vector3();
	gridDimensions	= Vector3();

	numTiles = 0;
}

void TileRenderer::GenerateGrid() 
{
	screenCube = Cube(Vector3(-1, -1, 0), Vector3(2, 2, 1));

	float xOffset = 0;
	float yOffset = 0;

	/*
	  Generate all tiles.
	*/
	for (int i = 0; i < numTiles; i += gridSize.z)
	{
		//Once reached the end of x axis, reset x offset and move up y axis.
		if (xOffset == gridSize.x)
		{
			yOffset += gridDimensions.y;
			xOffset = 0;
		}

		//Create tile closest to screen.
		Vector3 basePosition((gridDimensions.x * xOffset) + minCoord.x, yOffset + minCoord.y, 0);
		Vector3 baseDimensions(gridDimensions.x, gridDimensions.y, gridDimensions.z);

		Tile frontTile	= GenerateTile(basePosition, baseDimensions);

		grid[i] = Cube(basePosition, baseDimensions);
		screenTiles[i] = frontTile;

		//Fill along the z axis from the tile above.
		for (int k = 1; k <= gridSize.z - 1; ++k)
		{
			float newZ = gridDimensions.z * k;

			Tile newT = GenerateTile(Vector3(basePosition.x, basePosition.y, newZ), baseDimensions);

			grid[i + k] = Cube(Vector3(basePosition.x, basePosition.y, newZ), baseDimensions);
			screenTiles[i + k] = newT;
		}

		++xOffset;
	}
}

Tile TileRenderer::GenerateTile(Vector3 position, Vector3 dimensions) const
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

void TileRenderer::CullLights()
{
	lightsInFrustrum.clear();

	for (int l = 0; l < numLights; ++l)
	{
		//if (screenCube.SphereColliding(screenLightCoords[l], normalisedLightRadii[l]))
		//{
		//	lightsInFrustrum.push_back(l);
		//}
	}
}

void TileRenderer::FillTiles()
{
	/*
	  For each tile, check which light is within range.
	  Record:
	    - Which lights are intersecting (light indexes are global).
		- The number of intersections (this provides the length of the array in 2nd dimension).
	*/
	for (int t = 0; t < numTiles; ++t) 
	{
		int intersections = 0;

		for (int l = 0; l < numLights; ++l)
		{
			if (grid[t].SphereColliding(screenLightData[l]))
			{
				tileData.tileLights[t][intersections] = l;
				++intersections;
			}
		}

		tileData.lightIndexes[t] = intersections;
	}
}

void TileRenderer::PrepareData(const Matrix4& projectionMatrix, const Matrix4& viewMatrix)
{
	//Shared variables.
	Matrix4 projView = projectionMatrix * viewMatrix;
	Vector4 defaultPos(0.0f, 0.0f, 0.0f, 1.0f);

	Vector3 camPos = Vector3(viewMatrix.values[12], viewMatrix.values[13], viewMatrix.values[14]);
	float clipz = (projectionMatrix * camPos).z;

	//Fill data.
	for (int i = 0; i < numLights; ++i)
	{
		Vector4 viewPos = projView * lightModelMatrices[i] * defaultPos;

		//Store reciprocal to avoid use of division below.
		float w = 1 / viewPos.w;

		//Retrieve distance from camera to light + normalize.
		float ndcz = clipz * w * 100;

		screenLightData[i] = Vector4(viewPos.x * w, viewPos.y * w, ndcz, lights[i]->GetRadius() * w);
	}
}