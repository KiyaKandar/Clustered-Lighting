#version 430

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 viewMatrix;
uniform mat4 projMatrix;

//out vec3 FragPos;
out vec4 fragColour;

layout(std430, binding = 8) buffer modelMatricesBuffer
{
	mat4 modelMatrices[];
};

void main(void) 
{
	mat4 mvp = projMatrix * viewMatrix * modelMatrices[gl_InstanceID];
	gl_Position = mvp * vec4(aPos, 1.0);
	fragColour = vec4(0, 0, gl_Position.z, 1);

	//vec4 viewPos = viewMatrix * modelMatrices[gl_InstanceID] * vec4(aPos, 1.0);
	//FragPos = viewPos.xyz;
	//
	//gl_Position = projMatrix * viewPos;
}