#include "TileRenderer.h"

#include "Light.h"
#include "../Game/Util.h"
#include "../Game/GLUtil.h"

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

	gridPlanes = new CubePlanes[numTiles];

	compute = new ComputeShader(SHADERDIR"/Compute/compute.glsl", true);
	compute->LinkProgram();
	loc_numZTiles= glGetUniformLocation(compute->GetProgram(), "numZTiles");

	tileData = new TileData();

	InitGridSSBO();
}

TileRenderer::TileRenderer()
{
	numLights = 0;

	minCoord		= Vector2();
	gridSize		= Vector3();
	gridDimensions	= Vector3();

	numTiles = 0;

	compute = new ComputeShader(SHADERDIR"/Compute/compute.glsl", true);
	compute->LinkProgram();
	InitGridSSBO();
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

		CubePlanes baseGP;
		baseGP.faces[0] = Vector4(LEFT_NORMAL.x, LEFT_NORMAL.y, LEFT_NORMAL.z, basePosition.Length());
		baseGP.faces[1] = Vector4(RIGHT_NORMAL.x, RIGHT_NORMAL.y, RIGHT_NORMAL.z, (basePosition + Vector3(baseDimensions.x, 0, 0)).Length());
		baseGP.faces[2] = Vector4(FRONT_NORMAL.x, FRONT_NORMAL.y, FRONT_NORMAL.z, (basePosition + Vector3(0, 0, baseDimensions.z)).Length());
		baseGP.faces[3] = Vector4(BACK_NORMAL.x, BACK_NORMAL.y, BACK_NORMAL.z, basePosition.Length());
		baseGP.faces[4] = Vector4(TOP_NORMAL.x, TOP_NORMAL.y, TOP_NORMAL.z, basePosition.Length());
		baseGP.faces[5] = Vector4(BOTTOM_NORMAL.x, BOTTOM_NORMAL.y, BOTTOM_NORMAL.z, (basePosition + Vector3(0, baseDimensions.y, 0)).Length());

		baseGP.positions[0] = Vector4(basePosition.x, basePosition.y, basePosition.z, 0);
		baseGP.positions[1] = Vector4(basePosition.x + baseDimensions.x, basePosition.y, basePosition.z, 0);
		baseGP.positions[2] = Vector4(basePosition.x, basePosition.y, basePosition.z + baseDimensions.z, 0);
		baseGP.positions[3] = Vector4(basePosition.x, basePosition.y, basePosition.z, 0);
		baseGP.positions[4] = Vector4(basePosition.x, basePosition.y, basePosition.z, 0);
		baseGP.positions[5] = Vector4(basePosition.x, basePosition.y + baseDimensions.y, basePosition.z, 0);

		gridPlanes[i] = baseGP;

		screenTiles[i] = frontTile;

		//Fill along the z axis from the tile above.
		for (int k = 1; k <= gridSize.z - 1; ++k)
		{
			float newZ = gridDimensions.z * k;

			Vector3 newPosition(basePosition.x, basePosition.y, newZ);

			Tile newT = GenerateTile(newPosition, baseDimensions);

			grid[i + k] = Cube(newPosition, baseDimensions);

			CubePlanes newGP;
			newGP.faces[0] = Vector4(LEFT_NORMAL.x,		LEFT_NORMAL.y,	 LEFT_NORMAL.z,		newPosition.Length());
			newGP.faces[1] = Vector4(RIGHT_NORMAL.x,	RIGHT_NORMAL.y,  RIGHT_NORMAL.z,	(newPosition + Vector3(baseDimensions.x, 0, 0)).Length());
			newGP.faces[2] = Vector4(FRONT_NORMAL.x,	FRONT_NORMAL.y,  FRONT_NORMAL.z,	(newPosition + Vector3(0, 0, baseDimensions.z)).Length());
			newGP.faces[3] = Vector4(BACK_NORMAL.x,		BACK_NORMAL.y,	 BACK_NORMAL.z,		newPosition.Length());
			newGP.faces[4] = Vector4(TOP_NORMAL.x,		TOP_NORMAL.y,	 TOP_NORMAL.z,		newPosition.Length());
			newGP.faces[5] = Vector4(BOTTOM_NORMAL.x,	BOTTOM_NORMAL.y, BOTTOM_NORMAL.z,	(newPosition + Vector3(0, baseDimensions.y, 0)).Length());


			//newGP.position = Vector4(newPosition.x,		newPosition.y,	 newPosition.z,		0);
			newGP.positions[0] = Vector4(newPosition.x, newPosition.y, newPosition.z, 0);
			newGP.positions[1] = Vector4(newPosition.x + baseDimensions.x, newPosition.y, newPosition.z, 0);
			newGP.positions[2] = Vector4(newPosition.x, newPosition.y, newPosition.z + baseDimensions.z, 0);
			newGP.positions[3] = Vector4(newPosition.x, newPosition.y, newPosition.z, 0);
			newGP.positions[4] = Vector4(newPosition.x, newPosition.y, newPosition.z, 0);
			newGP.positions[5] = Vector4(newPosition.x, newPosition.y + baseDimensions.y, newPosition.z, 0);

			screenTiles[i + k] = newT;
		}

		++xOffset;
	}
}

void TileRenderer::InitGridSSBO()
{
	gridPlanesSSBO = GLUtil::InitSSBO(1, 4, gridPlanesSSBO,
		sizeof(CubePlanes) * numTiles, gridPlanes, GL_STATIC_COPY);

	screenSpaceDataSSBO = GLUtil::InitSSBO(1, 5, screenSpaceDataSSBO,
		sizeof(ScreenSpaceData), &ssdata, GL_STATIC_COPY);
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

void TileRenderer::FillTilesGPU()
{
	GLUtil::RebufferData(GL_SHADER_STORAGE_BUFFER, screenSpaceDataSSBO, 0, sizeof(ScreenSpaceData), &ssdata);

	//Writes to the shared buffer used in lighting pass
	compute->UseProgram();

	glUniform1i(loc_numZTiles, gridSize.z);

	compute->Compute(Vector3(1, 1, 1));
}

void TileRenderer::FillTilesCPU(GLuint buffer)
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
				tileData->tileLights[t][intersections] = l;
				++intersections;
			}
		}

		tileData->lightIndexes[t] = intersections;
	}

	GLUtil::RebufferData(GL_SHADER_STORAGE_BUFFER, buffer, 0, sizeof(TileData), tileData);
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

		//screenLightData[i] = Vector4(viewPos.x * w, viewPos.y * w, ndcz, lights[i]->GetRadius() * w);
		ssdata.data[i] = Vector4(viewPos.x * w, viewPos.y * w, ndcz, lights[i]->GetRadius() * w);
	}
}