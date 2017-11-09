#version 330 core

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedo;

uniform sampler2D texture_diffuse;

uniform int hasTexture;

in vec3 FragPos;
in vec2 TexCoords;
in vec3 Normal;

void main(void) {
	gPosition = FragPos;
	gNormal = normalize(Normal);

	vec4 col = vec4(0.0f);
	if (hasTexture == 1) 
	{
		col = texture2D(texture_diffuse, TexCoords);
	}
	else 
	{
		col = vec4(0.6f, 0.6f, 0.6f, 1.0f);
	}

	gAlbedo.rgba = col.rgba;
}