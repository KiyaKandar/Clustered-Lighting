#version 430 core

#include ../Shaders/compute/configuration.glsl

uniform float nearPlane;
uniform float farPlane;
uniform int numZTiles;
uniform int numLightsInFrustum;
uniform mat4 projMatrix;
uniform mat4 viewMatrix;
uniform vec4 cameraPosition;

const int GLOBAL_LIGHT = 0;

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

struct Tile
{
	float x;
	float y;
	float z;
	float width;
	float height;
	float length;

	float _padding[6];
};

//faces - xyz is normal, w is distance from origin
struct CubePlanes
{
	vec4 faces[6];
	vec4 positions[6];
};

struct LightData
{
	vec4 pos4;
	vec4 lightColour;
	float lightRadius;
	float intensity;

	float fpadding[2];
};

//Shared with lighting shader
layout(std430, binding = 1) buffer LightDataBuffer
{
	LightData lightData[];
};

layout (std430, binding = 3) buffer TileLightsBuffer
{
	int lightIndexes[numTiles];
	int tileLights[numTiles][numLights];
};

layout(std430, binding = 4) buffer CubePlanesBuffer
{
	CubePlanes cubePlanes[];
};

layout(std430, binding = 5) buffer ScreenSpaceDataBuffer
{
	float indexes[numLights];
	vec4 numLightsIn;
	vec4 NDCCoords[];
};

layout(std430, binding = 6) buffer ClipSpacePositionsBuffer
{
	vec4 ClipSpaceCoords[];
};

layout(binding = 0) uniform atomic_uint count;

#include ../Shaders/compute/collisionFunctions.glsl

void main()
{
	int xIndex = int(gl_GlobalInvocationID.x);
	int yIndex = int(gl_GlobalInvocationID.y);
	int zIndex = int(gl_GlobalInvocationID.z);

	int tile = xIndex + int(tilesOnAxes.x) * (yIndex + int(tilesOnAxes.y) * zIndex);

	uint index = uint(tile);

	int intersections = 0;

	for (int i = 0; i < numLights; i++)
	{
		if (true/*ClipSpaceCoords[i].w != 0.0f || i == GLOBAL_LIGHT*/)
		{
			if (i == GLOBAL_LIGHT || SphereCubeColliding(cubePlanes[index].faces, ClipSpaceCoords[i]))
			{
				tileLights[index][intersections] = i;
				intersections++;
			}
			else if (zIndex == 0)
			{
				if (PointInSphere(cameraPosition.xyz, ClipSpaceCoords[i], nearPlane, farPlane))
				{
					tileLights[index][intersections] = i;
					intersections++;
				}
			}
		}
	}

	lightIndexes[index] = intersections;
}

