#version 150 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 projMatrix;

in vec3 position;
in vec2 texCoord;

out Vertex {
	vec2 TexCoords;
} OUT;

void main(void) {
	gl_Position = projMatrix * vec4(position, 1.0);
	OUT.TexCoords = aTexCoords;
}