#version 430

uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat4 modelMatrix;

in vec3 position;

void main(void) {
	gl_Position 	  = (projMatrix * viewMatrix * modelMatrix) * vec4(position, 1.0);
}