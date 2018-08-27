#version 430

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat4 modelMatrix;

uniform sampler2D texture_height;
uniform int hasBumpMap;

out vec3 FragPos;
out vec3 reflectionPos;
out vec2 TexCoords;
out vec3 Normal;
out vec3 ReflectionNormal;

void main(void) 
{
	vec4 viewPos = viewMatrix * modelMatrix * vec4(aPos, 1.0);
	reflectionPos = vec3(modelMatrix * vec4(aPos, 1.0));
	FragPos = viewPos.xyz;
	TexCoords = aTexCoords;
	
	mat3 normalMatrix = transpose(inverse(mat3(viewMatrix * modelMatrix)));

	if (hasBumpMap == 1)
	{
		vec3 T = normalize(vec3(modelMatrix * vec4(aTangent, 0.0)));
		vec3 B = normalize(vec3(modelMatrix * vec4(aBitangent, 0.0)));
		vec3 N = normalize(vec3(modelMatrix * vec4(aNormal, 0.0)));

		if (dot(cross(N, T), B) < 0.0f)
		{
			T = T * -1.0f;
		}

		mat3 TBN = mat3(T, B, N);

		vec3 bumpMapNormal = normalize(texture2D(texture_height, aTexCoords).rgb * 2.0f - 1.0f);
		Normal = normalMatrix * (TBN * bumpMapNormal);
	}
	else
	{
		Normal = normalMatrix * (vec4(aNormal, 1.0)).xyz;
	}

	ReflectionNormal = mat3(transpose(inverse(modelMatrix))) * aNormal;

	gl_Position = projMatrix * viewPos;
}