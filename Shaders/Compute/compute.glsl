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
	vec4 position;
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

//bool SphereIntersecting(CubePlanes cube, vec4 light)
//{
//	float distSqr = light.w * light.w;
//
//	if (light.x < cube.faces[0)].x)
//	{
//		distSqr -= std::powf(spherePosition.x - faces[static_cast<int>(LEFT)].position.x, 2);
//	}
//	else if (spherePosition.x > faces[static_cast<int>(RIGHT)].position.x)
//	{
//		distSqr -= std::powf(spherePosition.x - faces[static_cast<int>(RIGHT)].position.x, 2);
//	}
//
//	if (spherePosition.y < faces[static_cast<int>(BOTTOM)].position.y)
//	{
//		distSqr -= std::powf(spherePosition.y - faces[static_cast<int>(BOTTOM)].position.y, 2);
//	}
//	else if (spherePosition.y > faces[static_cast<int>(TOP)].position.y)
//	{
//		distSqr -= std::powf(spherePosition.y - faces[static_cast<int>(TOP)].position.y, 2);
//	}
//
//	if (spherePosition.z < faces[static_cast<int>(FRONT)].position.z)
//	{
//		distSqr -= std::powf(spherePosition.z - faces[static_cast<int>(FRONT)].position.z, 2);
//	}
//	else if (spherePosition.z > faces[static_cast<int>(BACK)].position.z)
//	{
//		distSqr -= std::powf(spherePosition.z - faces[static_cast<int>(BACK)].position.z, 2);
//	}
//
//	return distSqr > 0;
//}

bool SphereColliding(CubePlanes cube, vec4 light)
{
	return (SphereInside(cube, light));
}


void main()
{
	//if (gl_GlobalInvocationID.x < 1000) 
	//{
	uint index = gl_GlobalInvocationID.x + 10 *
		(gl_GlobalInvocationID.y + 10 * gl_GlobalInvocationID.z);

		int intersections = 0;

		//tileLights[gl_GlobalInvocationID.x][0] = 0;

		for (int i = 0; i < 10; i++)
		{
			bool colliding = SphereColliding(cubePlanes[index], data[i]);

			if (colliding)
			{
			//tileLights[gl_GlobalInvocationID.x][intersections] = i;
			tileLights[index][intersections] = i;
			intersections++;
			}
		}

		lightIndexes[index] = intersections;
//	}

}

