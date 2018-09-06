#include "TileRenderer.h"

#include "../../../Utility/Light.h"
#include "../Game/Utility/Util.h"
#include "../Game/GraphicsConfiguration/GLUtil.h"
#include "../../Rendering/View/Camera.h"
#include "../../../Maths/Vector4.h"
#include "../../../Maths/Vector3.h"
#include "../../../Maths/Frustum.h"

const int INTERSECTING = 1;
const int EMPTY = 0;

TileRenderer::TileRenderer(Light** lights, int numLights, int numXTiles, int numYTiles, int numZTiles,
	Vector2 minScreenCoord, Vector2 maxScreenCoord, Camera* camera)
{
	this->camera = camera;
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
		1.0f / static_cast<float>(gridSize.z));

	numTiles = gridSize.x * gridSize.y * gridSize.z;

	gridPlanes = new CubePlanes[numTiles];
	clipSpaceLightPositions = new Vector4[GLConfig::NUM_LIGHTS];

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
	const GridData gridData(grid, gridPlanes, screenTiles, minCoord);
	GridUtility::Generate3DGrid(gridData, gridDimensions, gridSize);
}

void TileRenderer::InitGridSSBO()
{
	gridPlanesSSBO = GLUtil::InitSSBO(1, 4, gridPlanesSSBO,
		sizeof(CubePlanes) * numTiles, gridPlanes, GL_STATIC_COPY);

	screenSpaceDataSSBO = GLUtil::InitSSBO(1, 5, screenSpaceDataSSBO,
		sizeof(ScreenSpaceData), &ssdata, GL_STATIC_COPY);

	for (int i = 0; i < GLConfig::NUM_LIGHTS; ++i)
	{
		clipSpaceLightPositions[i] = Vector4(1, 1, 1, 1);
	}

	glGenBuffers(1, &clipSpaceSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, clipSpaceSSBO);
	glBufferStorage(GL_SHADER_STORAGE_BUFFER, sizeof(Vector4) * GLConfig::NUM_LIGHTS, clipSpaceLightPositions, GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT | GL_MAP_READ_BIT | GL_MAP_WRITE_BIT);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, clipSpaceSSBO);
	clipSpaceData = (Vector4*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, sizeof(Vector4) * GLConfig::NUM_LIGHTS, GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT | GL_MAP_READ_BIT | GL_MAP_WRITE_BIT);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void TileRenderer::AllocateLightsGPU(const Matrix4& projectionMatrix, const Matrix4& viewMatrix, 
	const Vector3& cameraPos, LightData* lightData, Frustum* frustum) const
{
	Matrix4 projView = projectionMatrix * viewMatrix;

	for (int i = 0; i < GLConfig::NUM_LIGHTS; ++i)
	{
		if (frustum->InsideFrustum(lightData[i].lightPosition.ToVector3(), lightData[i].lightRadius))
		{
			Vector4 worldPosition = Vector4(lightData[i].lightPosition.x, lightData[i].lightPosition.y, lightData[i].lightPosition.z, 1.0f);
			Vector4 projViewPos = projView * worldPosition;
			Vector4 viewPosition = viewMatrix * worldPosition;

			float zCoord = std::fabs(projViewPos.z) / (GLConfig::FAR_PLANE + GLConfig::NEAR_PLANE);
			float w = 1.0f / projViewPos.w;

			float radius = lightData[i].lightRadius * w;
			Vector4 clipSpacePosition(projViewPos.x * w, projViewPos.y * w, zCoord, radius);
			clipSpaceData[i] = clipSpacePosition;
		}
		else
		{
			clipSpaceData[i] = Vector4(0, 0, 0, 0);
		}
	}

	FillTilesGPU(projectionMatrix, viewMatrix);
}

void TileRenderer::FillTilesGPU(const Matrix4& projectionMatrix, const Matrix4& viewMatrix) const
{
	//Writes to the shared buffer used in lighting pass
	compute->UseProgram();

	glUniform1i(loc_numZTiles, gridSize.z);
	glUniform1i(glGetUniformLocation(compute->GetProgram(), "forceGlobalLight"), GLConfig::FORCE_GLOBAL_LIGHT_ZERO ? 1 : 0);
	glUniform1f(glGetUniformLocation(compute->GetProgram(), "nearPlane"), GLConfig::NEAR_PLANE);
	glUniform1f(glGetUniformLocation(compute->GetProgram(), "farPlane"), GLConfig::FAR_PLANE);
	glUniformMatrix4fv(glGetUniformLocation(compute->GetProgram(), "projMatrix"), 1, false, (float*)&projectionMatrix);
	glUniformMatrix4fv(glGetUniformLocation(compute->GetProgram(), "viewMatrix"), 1, false, (float*)&viewMatrix);

	Vector3 camPos = camera->GetPosition();
	float vec4[4] = { camPos.x, camPos.y, camPos.z, 0 };
	glUniform4fv(glGetUniformLocation(compute->GetProgram(), "cameraPosition"), 1, vec4);

	compute->Compute(Vector3(GLConfig::NUM_X_AXIS_CLUSTERS, GLConfig::NUM_Y_AXIS_CLUSTERS, GLConfig::NUM_Z_AXIS_CLUSTERS));

	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_ATOMIC_COUNTER_BARRIER_BIT);
}