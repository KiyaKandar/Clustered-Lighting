#version 430

//uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

in vec3 position;

layout(std430, binding = 8) buffer modelMatricesBuffer
{
	mat4 modelMatrices[];
};

void main(void) {
	gl_Position 	  = (projMatrix * viewMatrix * modelMatrices[gl_InstanceID]) * vec4(position, 1.0);
}