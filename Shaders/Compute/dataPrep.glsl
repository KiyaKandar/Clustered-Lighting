#version 430 core

const int numTiles = 1000;
const int numLights = 100;

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

float Length(vec3 v) 
{
	return sqrt((v.x*v.x) + (v.y*v.y) + (v.z*v.z));
}


vec3 LEFT_NORMAL = vec3(-1, 0, 0);
vec3 FRONT_NORMAL = vec3(0, 0, -1);
vec3 RIGHT_NORMAL = vec3(1, 0, 0);
vec3 BACK_NORMAL = vec3(0, 0, 1);
vec3 TOP_NORMAL = vec3(0, 1, 0);
vec3 BOTTOM_NORMAL = vec3(0, -1, 0);

vec3 screenPos = vec3(-1, -1, 0);
vec3 screenDimension = vec3(2, 2, 1);

vec4 splanesf[6] = vec4[6](
	vec4(LEFT_NORMAL.x, LEFT_NORMAL.y, LEFT_NORMAL.z, Length(screenPos)),
	vec4(RIGHT_NORMAL.x, RIGHT_NORMAL.y, RIGHT_NORMAL.z, Length(screenPos + vec3(screenDimension.x, 0, 0))),
	vec4(FRONT_NORMAL.x, FRONT_NORMAL.y, FRONT_NORMAL.z, Length(screenPos + vec3(0, 0, screenDimension.z))),
	vec4(BACK_NORMAL.x, BACK_NORMAL.y, BACK_NORMAL.z, Length(screenPos)),
	vec4(TOP_NORMAL.x, TOP_NORMAL.y, TOP_NORMAL.z, Length(screenPos + vec3(0, screenDimension.y, 0))),
	vec4(BOTTOM_NORMAL.x, BOTTOM_NORMAL.y, BOTTOM_NORMAL.z, Length(screenPos))
	);

vec4 splanesp[6] = vec4[6](
	vec4(screenPos.x, screenPos.y, screenPos.z, 0),
	vec4(screenPos.x + screenDimension.x, screenPos.y, screenPos.z, 0),
	vec4(screenPos.x, screenPos.y, screenPos.z + screenDimension.z, 0),
	vec4(screenPos.x, screenPos.y, screenPos.z, 0),
	vec4(screenPos.x, screenPos.y + screenDimension.y, screenPos.z, 0),
	vec4(screenPos.x, screenPos.y, screenPos.z, 0)
	);

CubePlanes screenCube = CubePlanes(splanesf, splanesp);

void main()
{

	const vec4 defaultPos = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	//CubePlanes screenCube = screenCube1;
	//barrier();

	//Create a model matrix for the light.
	//Translate to light position.
	mat4 model = mat4(1);
	model[0][3] = ldata[gl_GlobalInvocationID.x].pos4.x;
	model[1][3] = ldata[gl_GlobalInvocationID.x].pos4.y;
	model[2][3] = ldata[gl_GlobalInvocationID.x].pos4.z;
	//memoryBarrier();

	//move to screenspace.
	//vec4 clip = projectionMatrix * cameraPos;
	//float clipz = clip.z;
	vec4 worldLight = vec4(ldata[gl_GlobalInvocationID.x].pos4.xyz, 1);

	vec4 viewPos = projView * worldLight;//model * defaultPos;

	vec3 clipPos = viewPos.xyz / viewPos.w;

	//Store reciprocal to avoid use of division below.
	float w = 1.0f / viewPos.w;

	//Retrieve distance from camera to light + normalize.
	float ndcz = clipPos.z;//clipz * w;// *100;

	//Final screenspace data.
	vec4 result = vec4(viewPos.x * w, viewPos.y * w, ndcz, ldata[gl_GlobalInvocationID.x].lightRadius * w);
	//memoryBarrier();
	
	//Synchronise tor read screencube SSBO
	//memoryBarrier();
	//CubePlanes localscreencube = screenCube1;

	bool colliding = SphereColliding(screenCube, result);

	//If light affects any clusters on screen, send to next shader for allocation, 
	//else cull.
	if (colliding) {
	//if (gl_GlobalInvocationID.x == 0) {
	//if (true) {
			uint currentLightCount = atomicCounterIncrement(count);
			data[currentLightCount] = result;
			indexes[currentLightCount] = gl_GlobalInvocationID.x;


			//memoryBarrier();
			//data[gl_GlobalInvocationID.x] = result;
			//atomicExchange(data[0].x, result.x);
			//memoryBarrier();
			//indexes[gl_GlobalInvocationID.x] =  gl_GlobalInvocationID.x;
			//atomicExchange(indexes[0], 0);
			//memoryBarrier();
		//}
	}

			//barrier();
}

