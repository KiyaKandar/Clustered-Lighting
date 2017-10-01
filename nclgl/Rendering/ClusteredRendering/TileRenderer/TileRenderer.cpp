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
		1 / static_cast<float>(gridSize.z));

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
	loc_numZTiles = glGetUniformLocation(compute->GetProgram(), "numZTiles");

	dataPrep = new ComputeShader(SHADERDIR"/Compute/dataPrep.glsl", true);
	dataPrep->LinkProgram();
	loc_projMatrix = glGetUniformLocation(dataPrep->GetProgram(), "projectionMatrix");
	loc_projView = glGetUniformLocation(dataPrep->GetProgram(), "projView");
	loc_cameraPos = glGetUniformLocation(dataPrep->GetProgram(), "cameraPos");

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

	compute = new ComputeShader(SHADERDIR"/Compute/compute.glsl", true);
	compute->LinkProgram();

	dataPrep = new ComputeShader(SHADERDIR"/Compute/dataPrep.glsl", true);
	dataPrep->LinkProgram();

	InitGridSSBO();
}

void TileRenderer::GenerateGrid()
{
	const Vector3 screenPos(-1, -1, 0);
	const Vector3 screenDimension(2, 2, 1);

	screenCube = Cube(screenPos, screenDimension);
	screenPlanes = GridUtility::GenerateCubePlanes(screenPos, screenDimension);

	const GridData gridData(grid, gridPlanes, screenTiles, minCoord);
	GridUtility::Generate3DGrid(gridData, gridDimensions, gridSize);
}

void TileRenderer::InitGridSSBO()
{
	gridPlanesSSBO = GLUtil::InitSSBO(1, 4, gridPlanesSSBO,
		sizeof(CubePlanes) * numTiles, gridPlanes, GL_STATIC_COPY);

	screenSpaceDataSSBO = GLUtil::InitSSBO(1, 5, screenSpaceDataSSBO,
		sizeof(ScreenSpaceData), &ssdata, GL_STATIC_COPY);

	screenCubeSSBO = GLUtil::InitSSBO(1, 6, screenCubeSSBO,
		sizeof(CubePlanes), &screenPlanes, GL_STATIC_COPY);

	glGenBuffers(1, &countBuffer);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, countBuffer);
	glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), NULL, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);
	glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, countBuffer);

}

void TileRenderer::AllocateLightsCPU(const Matrix4& projectionMatrix,
	const Matrix4& viewMatrix, GLuint buffer, const Vector3& cameraPos)
{
	PrepareDataCPU(projectionMatrix, viewMatrix, cameraPos);
	CullLights();
	FillTilesCPU(buffer);
}

void TileRenderer::AllocateLightsGPU(const Matrix4& projectionMatrix, const Matrix4& viewMatrix, 
	const Vector3& cameraPos) const
{
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, countBuffer);
	glInvalidateBufferData(countBuffer);
	GLuint zero = 0;
	glClearBufferData(GL_ATOMIC_COUNTER_BUFFER, GL_R32UI, GL_RED_INTEGER, GL_UNSIGNED_INT, &zero);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);

	PrepareDataGPU(projectionMatrix, viewMatrix, cameraPos);
	FillTilesGPU();
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

	for (int i = 0; i < numLights; ++i)
	{
		const Vector4 worldLight = Vector4(lights[i]->GetPosition().x, lights[i]->GetPosition().y, lights[i]->GetPosition().z, 1.0f);
		const Vector4 viewPos = projView * worldLight;
		const Vector3 clipPos = Vector3(viewPos.x, viewPos.y, viewPos.z) / viewPos.w;

		//Store reciprocal to avoid use of division below.
		const float w = 1 / viewPos.w;

		//Retrieve distance from camera to light + normalize.
		const float ndcz = clipPos.z;

		screenLightData[i] = Vector4(viewPos.x * w, viewPos.y * w, ndcz, lights[i]->GetRadius() * w);
	}
}

void TileRenderer::FillTilesGPU() const
{
	//Writes to the shared buffer used in lighting pass
	compute->UseProgram();

	glUniform1i(loc_numZTiles, gridSize.z);

	compute->Compute(Vector3(1, 1, 1));

	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_ATOMIC_COUNTER_BARRIER_BIT);
}

void TileRenderer::PrepareDataGPU(const Matrix4& projectionMatrix, const Matrix4& viewMatrix, 
	const Vector3& cameraPos) const 
{
	Matrix4 projView = projectionMatrix * viewMatrix;
	const Vector4 camPos = Vector4(viewMatrix.values[12], viewMatrix.values[13], viewMatrix.values[14], 0);
	dataPrep->UseProgram();

	glUniformMatrix4fv(loc_projMatrix, 1, false, (float*)&projectionMatrix);
	glUniformMatrix4fv(loc_projView, 1, false, (float*)&projView);

	float vec4[4] = { camPos.x, camPos.y, camPos.z, 0 };
	glUniform4fv(loc_cameraPos, 1, vec4);

	dataPrep->Compute(Vector3(2, 1, 1));
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_ATOMIC_COUNTER_BARRIER_BIT);
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