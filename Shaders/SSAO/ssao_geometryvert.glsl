#version 430

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 viewMatrix;
uniform mat4 projMatrix;

out vec3 FragPos;
out vec3 reflectionPos;
out vec2 TexCoords;
out vec3 Normal;
out vec3 ReflectionNormal;

layout(std430, binding = 8) buffer modelMatricesBuffer
{
	mat4 modelMatrices[];
};

void main(void) 
{
	vec4 viewPos = viewMatrix * modelMatrices[gl_InstanceID] * vec4(aPos, 1.0);
	reflectionPos = vec3(modelMatrices[gl_InstanceID] * vec4(aPos, 1.0));
	FragPos = viewPos.xyz;
	TexCoords = aTexCoords;
	
	mat3 normalMatrix = transpose(inverse(mat3(viewMatrix * modelMatrices[gl_InstanceID])));
	Normal = normalMatrix * (vec4(aNormal, 1.0)).xyz;
	ReflectionNormal = mat3(transpose(inverse(modelMatrices[gl_InstanceID]))) * aNormal;

	gl_Position = projMatrix * viewPos;
}