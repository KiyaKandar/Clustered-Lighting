#version 330 core

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedo;

uniform sampler2D texture_diffuse;
uniform int hasTexture;

uniform vec3 cameraPos;
uniform vec4 baseColour;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform int isReflective;
uniform float reflectionStrength;
uniform samplerCube skybox;

in vec3 FragPos;
in vec2 TexCoords;
in vec3 Normal;
in vec3 ReflectionNormal;
in vec3 reflectionPos;

void main(void) {
	gPosition = FragPos;
	gNormal = normalize(Normal);

	vec4 col = baseColour;
	float alpha = col.a;

	if (hasTexture == 1) 
	{
		col = texture2D(texture_diffuse, TexCoords);
	}
	
	if (isReflective == 1) 
	{
		vec3 I = normalize(reflectionPos - cameraPos);
		vec3 R = reflect(I, normalize(ReflectionNormal));
		vec4 reflectionColour = vec4(texture(skybox, R).rgb, 1.0);
		col += reflectionColour * reflectionStrength;
		col /= 2;
	}

	gAlbedo.rgba = vec4(col.rgb, alpha);
}