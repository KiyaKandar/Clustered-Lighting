#version 430 core

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

layout (std430, binding = 3) buffer TileLightsBuffer
{
	int lightIndexes[1000];
	int tileLights[1000][10];
};

layout(std430, binding = 4) buffer CubePlanesBuffer
{
	CubePlanes cubePlanes[1000];
};

layout(std430, binding = 5) buffer ScreenSpaceDataBuffer
{
	vec4 data[10];
};

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

void main()
{
	uint index = gl_GlobalInvocationID.x + 10 *
		(gl_GlobalInvocationID.y + 10 * gl_GlobalInvocationID.z);

	int intersections = 0;

	for (int i = 0; i < 10; i++)
	{
		bool colliding = SphereColliding(cubePlanes[index], data[i]);

		if (colliding)
		{
		tileLights[index][intersections] = i;
		intersections++;
		}
	}

	lightIndexes[index] = intersections;
}

