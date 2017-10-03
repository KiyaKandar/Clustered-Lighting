#include "TileRenderer.h"

#include "../../../Utility/Light.h"
#include "../Game/Utility/Util.h"
#include "../Game/GraphicsConfiguration/GLUtil.h"

const int INTERSECTING = 1;
const int EMPTY = 0;

TileRenderer::TileRenderer(Light** lights, int numLights, int numXTiles, int numYTiles, int numZTiles,
	Vector2 minScreenCoord, Vector2 maxScreenCoord)
{
	this->numLights = numLights;
	this->minCoord = minScreenCoord;
	this->lights = lights;

	//Check number of tiles is greater than 0
	Util::AssertGreaterThan<int>(numXTiles, 0, false, THROW_ERROR);
	Util::AssertGreaterThan<int>(numYTiles, 0, false, THROW_ERROR);
	Util::AssertGreaterThan<int>(numZTiles, 0, false, THROW_ERROR);

	gridSize = Vector3(numXTiles, numYTiles, numZTiles);
	gridDimensions = Vector3(
		std::abs(minScreenCoord.x - maxScreenCoord.x) / static_cast<float>(gridSize.x),
		std::abs(minScreenCoord.y - maxScreenCoord.y) / static_cast<float>(gridSize.y),
		15000.0f / static_cast<float>(gridSize.z));

	numTiles = gridSize.x * gridSize.y * gridSize.z;

	for (int i = 0; i < numLights; ++i)
	{
		Matrix4 modelMat;
		modelMat.SetPositionVector(lights[i]->GetPosition());

		lightModelMatrices[i] = modelMat;
	}

	gridPlanes = new CubePlanes[numTiles];

	tileData = new TileData();
	GenerateGrid();
	InitGridSSBO();
}

TileRenderer::TileRenderer()
{
	numLights = 0;

	minCoord = Vector2();
	gridSize = Vector3();
	gridDimensions = Vector3();

	numTiles = 0;

	InitGridSSBO();
}

void TileRenderer::GenerateGrid()
{
	const Vector3 screenPos(-1.0f, -1.0f, 1.0f);
	const Vector3 screenDimension(2, 2, 15000.0f);

	screenCube = Cube(screenPos, screenDimension);

	const GridData gridData(grid, gridPlanes, screenTiles, minCoord);
	GridUtility::Generate3DGrid(gridData, gridDimensions, gridSize);
}

void TileRenderer::InitGridSSBO()
{
	screenSpaceDataSSBO = GLUtil::InitSSBO(1, 5, screenSpaceDataSSBO,
		sizeof(ScreenSpaceData), &ssdata, GL_STATIC_COPY);
}

void TileRenderer::AllocateLightsCPU(const Matrix4& projectionMatrix,
	const Matrix4& viewMatrix, GLuint buffer, const Vector3& cameraPos)
{
	PrepareDataCPU(projectionMatrix, viewMatrix, cameraPos);
	CullLights();
	FillTilesCPU(buffer);
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

		for (int l = 0; l < numLightsInFrustum; ++l)
		{
			if (grid[t].SphereColliding(ssdata.data[l]))
			{
				tileData->tileLights[t][intersections] = inds[l];
				++intersections;
			}
		}

		tileData->lightIndexes[t] = intersections;
	}
}

void TileRenderer::PrepareDataCPU(const Matrix4& projectionMatrix, const Matrix4& viewMatrix, const Vector3& cameraPos)
{
	const Matrix4 projView = projectionMatrix * viewMatrix;
	//const Matrix4 projView = viewMatrix;

	for (int i = 0; i < numLights; ++i)
	{
		const Vector4 worldLight = Vector4(lights[i]->GetPosition().x, lights[i]->GetPosition().y, lights[i]->GetPosition().z, 1.0f);
		const Vector4 viewPos = projView * worldLight;

		//Store reciprocal to avoid use of division below.
		const float w = 1.f / viewPos.w;

		//Retrieve distance from camera to light + normalize.
		const float ndcz = viewPos.z;// clipPos.z; // 

		screenLightData[i] = Vector4(viewPos.x * w, viewPos.y * w, ndcz, lights[i]->GetRadius() * w);
	}
}

void TileRenderer::CullLights()
{
	numLightsInFrustum = 0;

	for (int i = 0; i < numLights; ++i)
	{
		if (screenCube.SphereColliding(screenLightData[i]))
		{
			ssdata.data[numLightsInFrustum] = screenLightData[i];
			ssdata.indexes[numLightsInFrustum] = i;
			inds[numLightsInFrustum] = i;

			++numLightsInFrustum;
		}
	}

	ssdata.numLightsIn.x = numLightsInFrustum;
}