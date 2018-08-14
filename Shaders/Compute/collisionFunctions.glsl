bool SphereInPlane(vec4 plane, vec4 light)
{
	if (dot(light.xyz, plane.xyz) + plane.w <= -light.w)
	{
		return false;
	}
	
	return true;
}

bool SphereColliding(CubePlanes cube, vec4 light, mat4 projMatrix, mat4 viewMatrix)
{
	for (int i = 0; i < 6; i++)
	{
		//vec4 temp = vec4(cube.positions[i].xyz, 1);
		//temp = inverse(projMatrix * viewMatrix) * temp;
		//vec3 worldSpace = temp.xyz / temp.w;
		////vec3 worldSpace = (inverse(viewMatrix) * vec4(clipSpace, 1.0f)).xyz;

		//vec4 clusterPlane = vec4(cube.faces[i].xyz, length(worldSpace));

		if (!SphereInPlane(cube.faces[i], light))
		{
			return false;
		}
	}

	return true;
}