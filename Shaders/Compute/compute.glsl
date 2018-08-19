#version 430 core

#include ../Shaders/compute/configuration.glsl

uniform int numZTiles;
uniform int numLightsInFrustum;
uniform mat4 projMatrix;
uniform mat4 viewMatrix;

layout(local_size_x = 10, local_size_y = 10, local_size_z = 10) in;

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

layout(binding = 0) uniform atomic_uint count;

//TEMP
layout(binding = 1) uniform atomic_uint intersectionCount;
//

#include ../Shaders/compute/collisionFunctions.glsl

void main()
{
	int xIndex = int(gl_GlobalInvocationID.x);
	int yIndex = int(gl_GlobalInvocationID.y);
	int zIndex = int(gl_GlobalInvocationID.z);

	int tile = xIndex + int(tilesOnAxes.x) * (yIndex + int(tilesOnAxes.y) * zIndex);//xIndex + (yIndex * int(tilesOnAxes.x)) + (zIndex * (int(tilesOnAxes.x * tilesOnAxes.y)));

	uint index = uint(tile);

	int intersections = 0;

	uint lightsOnScreen = atomicCounter(count);
	for (int i = 0; i < lightsOnScreen; i++)
	{
		int lightIndex = int(indexes[i]);

		if (SphereColliding(cubePlanes[index], NDCCoords[i], projMatrix, viewMatrix))
		{
			tileLights[index][intersections] = lightIndex;
			intersections++;

			//TEMP
			atomicCounterIncrement(intersectionCount);
			//
		}
	}

	lightIndexes[index] = intersections;
}

