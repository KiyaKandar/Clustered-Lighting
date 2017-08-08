#version 330 core

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec3 gAlbedo;

uniform sampler2D texture_diffuse;

in vec3 FragPos;
in vec2 TexCoords;
in vec3 Normal;

void main(void) {
	gPosition = FragPos;
	gNormal = normalize(Normal);

	vec4 col = texture2D(texture_diffuse, TexCoords);

	gAlbedo.rgb = col.rgb;
}