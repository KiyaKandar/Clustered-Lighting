bool SphereInPlane(vec4 plane, vec4 light)
{
	if (dot(light.xyz, plane.xyz) + plane.w <= -light.w)
	{
		return false;
	}

	return true;

	//float dotResult = -(dot(plane.xyz, plane.w));
	//float lengthResult = dot(plane.w, light.xyz + plane.xyz);
	//float distanceResult = dot(vec3(plane.w, 1, 1), light.xyz) + length(plane.xyz);

	//if (distanceResult <= light.w)
	//{
	//	return true;
	//}

	//return false;

	//vec3 distance = light.xyz - plane.xyz;
	//float dist = dot(distance, vec3(plane.w, 1, 1));

	//if (dist <= light.w)
	//{
	//	return true;
	//}

	//return false;
}

bool SphereInside(CubePlanes cube, vec4 light)
{
	//int outsideCount = 0;
	for (int i = 0; i < 6; i++)
	{
		if (!SphereInPlane(cube.faces[i], light))
		{
			//outsideCount++;
			return false;
		}
	}

	//if (outsideCount > 2)
	//{
	//	return false;
	//}

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
	//return (SphereInside(cube, light) || SphereIntersecting(cube, light));
	return SphereInside(cube, light);
}

float Length(vec3 v) 
{
	//return sqrt((v.x*v.x) + (v.y*v.y) + (v.z*v.z));
	return length(v);
}