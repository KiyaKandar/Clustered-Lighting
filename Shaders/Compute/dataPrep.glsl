#version 430 core

#include ../Shaders/compute/configuration.glsl

uniform float nearPlane;
uniform float farPlane;
uniform mat4 projectionMatrix;
uniform mat4 projView;
uniform mat4 viewMatrix;

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
	float bulbRadius;
	float lightCutoffRadius;
	float intensity;

	float fpadding;
};

//Shared with lighting shader
layout(std430, binding = 1) buffer LightDataBuffer
{
	LightData lightData[];
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
	vec4 numLightsIn;
	vec4 NDCCoords[];
};

layout(binding = 0) uniform atomic_uint count;

#include ../Shaders/compute/collisionFunctions.glsl

void main()
{
	int xIndex = int(gl_GlobalInvocationID.x);
	int yIndex = int(gl_GlobalInvocationID.y);
	int zIndex = int(gl_GlobalInvocationID.z);

	int id = xIndex + int(gl_NumWorkGroups.x) * (yIndex + int(gl_NumWorkGroups.y) * zIndex);

	vec4 worldLight = vec4(lightData[id].pos4.xyz, 1.0f);

	vec4 frustum[6];
	FrustumFromMatrix(projView, frustum);
	bool colliding = SphereCubeColliding(frustum, vec4(worldLight.xyz, lightData[id].lightCutoffRadius));

	//If light affects any clusters on screen, send to next shader for allocation, 
	//else cull.
	if (colliding)
	{
		vec4 projViewPos = projView * worldLight;
		vec4 viewPos = viewMatrix * worldLight;
		float zCoord = abs(viewPos.z - nearPlane) / farPlane + nearPlane;

		//Store reciprocal to avoid use of division below.
		float w = 1.0f / projViewPos.w;

		//Final screenspace data.
		float radius = lightData[id].lightCutoffRadius * w;
		vec4 clipPos = vec4(projViewPos.x * w, projViewPos.y * w, zCoord, radius);

		uint currentLightCount = atomicCounterIncrement(count);

		NDCCoords[currentLightCount] = clipPos;
		indexes[currentLightCount] = id;
	}
}

