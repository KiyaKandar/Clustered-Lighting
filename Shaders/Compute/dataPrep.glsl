#version 430 core

#include ../Shaders/compute/configuration.glsl

uniform mat4 projectionMatrix;
uniform mat4 projView;
uniform vec4 cameraPos;

layout(local_size_x = 50, local_size_y = 1, local_size_z = 1) in;

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
	LightData ldata[];
};

//Shared with compute shader
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
	//float padding[9];

	vec4 numLightsIn;
	vec4 data[];
};

layout(std430, binding = 6) buffer ScreenCubeBuffer
{
	CubePlanes screenCube1;
};

layout(binding = 0) uniform atomic_uint count;

#include ../Shaders/compute/collisionFunctions.glsl
#include ../Shaders/compute/screenCube.glsl

CubePlanes screenCube = CubePlanes(splanesf, splanesp);

void main()
{
	//move to screenspace.
	vec4 worldLight = vec4(ldata[gl_GlobalInvocationID.x].pos4.xyz, 1);
	vec4 viewPos = projView * worldLight;

	//Store reciprocal to avoid use of division below.
	float w = 1.0f / viewPos.w;

	//Final screenspace data.
	vec4 result = vec4(viewPos.x * w, viewPos.y * w, viewPos.z, ldata[gl_GlobalInvocationID.x].lightRadius * w);

	bool colliding = SphereColliding(screenCube, result);

	//If light affects any clusters on screen, send to next shader for allocation, 
	//else cull.
	if (colliding) 
	{
		uint currentLightCount = atomicCounterIncrement(count);
		data[currentLightCount] = result;
		indexes[currentLightCount] = gl_GlobalInvocationID.x;
	}
}

