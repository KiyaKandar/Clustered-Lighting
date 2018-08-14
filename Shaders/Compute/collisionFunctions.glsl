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
		//vec4 NDCposition = vec4(cube.positions[i].xyz, 1);
		//float posW = vec4(inverse(projMatrix) * NDCposition).w;
		//vec3 clipSpacePosition = NDCposition.xyz / posW;

		//vec4 NDCNormal = vec4(cube.faces[i].xyz, 1);
		//float normalW = vec4(inverse(projMatrix) * NDCNormal).w;
		//vec3 clipSpaceNormal = NDCNormal.xyz / normalW;
		////viewNormal = viewMatrix * vec4(viewNormal, 1.0f);

		//vec4 clusterPlane = vec4(cube.faces[i].xyz, length(cube.positions[i].xyz));

		if (!SphereInPlane(cube.faces[i], light))
		{
			return false;
		}
	}

	return true;
}
//vec3 worldSpace = (inverse(viewMatrix) * vec4(clipSpace, 1.0f)).xyz;








//vec4 finalPosition = inverse(projMatrix) * vec4(clipSpacePosition, 1.0f);
//vec4 finalNormal = inverse(projMatrix) * vec4(clipSpaceNormal, 1.0f);

//finalPosition = viewMatrix * finalPosition;
//finalNormal = viewMatrix * finalNormal;

//vec4 viewSpaceNormal = viewMatrix * vec4(clipSpaceNormal, 1.0f);