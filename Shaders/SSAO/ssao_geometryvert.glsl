#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

out vec3 FragPos;
out vec2 TexCoords;
out vec3 Normal;

void main(void) 
{
	vec4 viewPos = viewMatrix * modelMatrix * vec4(aPos, 1.0);
	FragPos = viewPos.xyz;
	TexCoords = aTexCoords;
	
	mat3 normalMatrix = transpose(inverse(mat3(viewMatrix * modelMatrix)));
	Normal = normalMatrix * (vec4(aNormal, 1.0)).xyz;

	gl_Position = projMatrix * viewPos;
}