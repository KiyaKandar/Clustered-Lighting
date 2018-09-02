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

layout(local_size_x = 2, local_size_y = 2, local_size_z = 2) in;

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
	float bulbRadius;
	float lightCutoffRadius;
	float intensity;

	float padding;
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

#include ../Shaders/compute/collisionFunctions.glsl

void main()
{
	int xIndex = int(gl_GlobalInvocationID.x);
	int yIndex = int(gl_GlobalInvocationID.y);
	int zIndex = int(gl_GlobalInvocationID.z);

	uint index = uint((int(tilesOnAxes.y) * int(tilesOnAxes.x) * zIndex) + (int(tilesOnAxes.x) * yIndex) + xIndex);

	int intersections = 0;

	for (int i = 0; i < numLights; ++i)
	{
		int lightIndex = int(indexes[i]);

		if (lightIndex >= 0)
		{
			if (SphereCubeColliding(cubePlanes[index].faces, NDCCoords[i]) || PointInSphere(cameraPosition.xyz, NDCCoords[i], nearPlane, farPlane))
			{
				tileLights[index][intersections] = lightIndex;
				intersections++;
			}
		}
	}

	lightIndexes[index] = intersections;
}

