#version 430 core

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

struct CubePlanes
{
	vec4 faces[6];
	vec4 position;
};

layout (std430, binding = 4) buffer TileLightsBuffer
{
	int lightIndexes[1000];
	int tileLights[1000][10];
};

layout(std430, binding = 5) buffer CubePlanesBuffer
{
	CubePlanes cubePlanes[1000];
};

layout(std430, binding = 6) buffer ScreenSpaceDataBuffer
{
	vec4 data[10];
};

layout(local_size_x = 32, local_size_y = 1, local_size_z = 1) in;

//bool SphereColliding(CubePlanes cube, vec4 light)
//{
//	int i = 0;
//
//	return true;
//}

void main()
{
	if (gl_GlobalInvocationID.x < 1000) 
	{
		int intersections = 0;

		//tileLights[gl_GlobalInvocationID.x][0] = 0;

		for (int i = 0; i < 10; i++)
		{
			//bool colliding = SphereColliding(cubePlanes[gl_GlobalInvocationID.x], data[i]);

			//if (colliding)
			//{
			//tileLights[gl_GlobalInvocationID.x][intersections] = i;
			tileLights[gl_GlobalInvocationID.x][intersections] = i;
			intersections++;
			//}
		}

		lightIndexes[gl_GlobalInvocationID.x] = intersections;
	}

}

