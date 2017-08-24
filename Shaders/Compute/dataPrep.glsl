#version 430 core

const int numTiles = 1000;
const int numLights = 100;

uniform mat4 projectionMatrix;
uniform mat4 projView;
uniform vec4 cameraPos;

layout(local_size_x = 20, local_size_y = 1, local_size_z = 1) in;

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
	float indexes[100];
	//float padding[9];

	vec4 numLightsIn;
	vec4 data[];
};
//
//layout(std430, binding = 6) buffer LightModelMatricesBuffer
//{
//	mat4 modelMatrices[];
//};

layout(std430, binding = 7) buffer ScreenCubeBuffer
{
	CubePlanes screenCube;
};

layout(binding = 0) uniform atomic_uint count;

bool SphereInPlane(vec4 plane, vec4 light)
{
	if (dot(light.xyz, plane.xyz) + plane.w <= -light.w)
	{
		return false;
	}

	return true;
}

bool SphereInside(CubePlanes cube, vec4 light)
{
	for (int i = 0; i < 6; i++)
	{
		if (!SphereInPlane(cube.faces[i], light))
		{
			return false;
		}
	}

	return true;
}

bool SphereIntersecting(CubePlanes cube, vec4 light)
{
	float distSqr = light.w * light.w;

	if (light.x < cube.positions[0].x)
	{
		distSqr -= pow(light.x - cube.positions[0].x, 2);
	}
	else if (light.x > cube.positions[1].x)
	{
		distSqr -= pow(light.x - cube.positions[1].x, 2);
	}

	if (light.y < cube.positions[5].y)
	{
		distSqr -= pow(light.y - cube.positions[5].y, 2);
	}
	else if (light.y > cube.positions[4].y)
	{
		distSqr -= pow(light.y - cube.positions[4].y, 2);
	}

	if (light.z < cube.positions[2].z)
	{
		distSqr -= pow(light.z - cube.positions[2].z, 2);
	}
	else if (light.z > cube.positions[3].z)
	{
		distSqr -= pow(light.z - cube.positions[3].z, 2);
	}

	return distSqr > 0;
}

bool SphereColliding(CubePlanes cube, vec4 light)
{
	return (SphereInside(cube, light) || SphereIntersecting(cube, light));
}

shared CubePlanes cube;// = screenCube;

void main()
{
	//cube = screenCube;
	//Shared variables.
	//mat4 projView = projMatrix * viewMatrix;
	vec4 defaultPos = vec4(0.0f, 0.0f, 0.0f, 1.0f);

	//Vector3 camPos = Vector3(viewMatrix.values[12], viewMatrix.values[13], viewMatrix.values[14]);
	
	//vec4 values = vec4(viewMatrix[0][3], viewMatrix[1][3], viewMatrix[2][3], 0);
	
	mat4 model;
	model[0][3] = ldata[gl_GlobalInvocationID.x].pos4.x;
	model[1][3] = ldata[gl_GlobalInvocationID.x].pos4.y;
	model[2][3] = ldata[gl_GlobalInvocationID.x].pos4.z;

	vec4 clip = projectionMatrix * cameraPos;
	float clipz = clip.z;

	//Fill data.
	vec4 viewPos = projView * model * defaultPos;

	//Store reciprocal to avoid use of division below.
	float w = 1 / viewPos.w;

	//Retrieve distance from camera to light + normalize.
	float ndcz = clipz * w * 100;

	vec4 result = vec4(viewPos.x * w, viewPos.y * w, ndcz, ldata[gl_GlobalInvocationID.x].lightRadius * w);

	memoryBarrier();
	bool colliding = SphereColliding(screenCube, result);

	if (colliding)
	{
		uint currentLightCount = atomicCounterIncrement(count);

		data[currentLightCount] = result;
		indexes[currentLightCount] = gl_GlobalInvocationID.x;
	}
}

