#version 330 core

layout (location = 0) in vec3 aPos;

uniform mat4 viewMatrix;
uniform mat4 projMatrix;

out vec3 TexCoords;

void main(void) 
{
	mat4 viewMatrixWithoutTranslation = mat4(mat3(viewMatrix));
	TexCoords = aPos;
	vec4 position = projMatrix * viewMatrixWithoutTranslation * vec4(aPos * 10, 1.0);
    gl_Position = position.xyww;
}