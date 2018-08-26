#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoords;
layout(location = 2) in vec3 aNormal;

uniform mat4 textureMatrix;

out vec2 TexCoords;
out vec2 screenPos;

void main()
{
    TexCoords = aTexCoords;

	screenPos = vec2(aPos.x, aPos.y);

    gl_Position = vec4(aPos, 1.0);

}