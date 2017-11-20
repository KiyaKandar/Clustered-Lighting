#version 330 core

uniform samplerCube skybox;

out vec4 FragColor;
in vec3 TexCoords;

void main(void) 
{
	FragColor = texture(skybox, TexCoords);
}