#version 430

layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 BrightnessCol;

#include ../Shaders/compute/configuration.glsl

uniform int numShadowCastingLights;

uniform vec3  cameraPos;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D ssao;

uniform sampler2DShadow shadows[5];
uniform mat4 texMatrices[5];
uniform mat4 camMatrix;

uniform sampler2D ambientTextures[1];

uniform int numberOfLights;
uniform int numXTiles;
uniform int numYTiles;

in vec2 TexCoords;
in mat4 textureMat;
in vec2 screenPos;

struct LightData
{
	vec4 pos4;
	vec4 lightColour;
	float lightRadius;
	float intensity;

	float fpadding[2];
};

struct Tile
{
	float x;
	float y;
	float z;
	float width;
	float height;
	float length;

	float _padding[6];
};

layout (std430, binding = 1) buffer LightDataBuffer
{
	LightData data[];
};

layout (std430, binding = 2) buffer TileDataBuffer
{
	Tile screenTiles[];
};

layout (std430, binding = 3) buffer TileLightsBuffer
{
	int lightIndexes[numTiles];
	int tileLights[numTiles][numLights];
};

void main(void){
    //Retrieve data from gbuffer
    vec3 position	= texture(gPosition, TexCoords).rgb;
    vec3 normal		= normalize(texture(gNormal, TexCoords).rgb);
	vec3 albedoCol = vec3(0.6f, 0.6f, 0.6f);// texture(gAlbedo, TexCoords).rgb; // 

	//Transform screenspace coordinates into a tile index
	float xCoord = gl_FragCoord.x / 1280;
	float yCoord = gl_FragCoord.y / 720;
	float zCoord = gl_FragCoord.z;

	zCoord = (position.z - 1.0f) / (15000.0f - 1.0f);
	zCoord = abs(zCoord);

	int xIndex = int(xCoord * tilesOnAxes.x);
	int yIndex = int(yCoord * tilesOnAxes.y);
	int zIndex = int(zCoord * tilesOnAxes.z);

	int tile = xIndex + (yIndex * int(tilesOnAxes.y)) + (zIndex * (int(tilesOnAxes.x * tilesOnAxes.z)));

	//Default value
	vec3 lightResult = vec3(0.0, 0.0, 0.0);

	for(int j = 0; j < lightIndexes[tile]; j++)
	{
		int lightIndex = tileLights[tile][j];

		vec3 lightPosition = data[lightIndex].pos4.xyz;

		vec3 colour = vec3(0.0, 0.0, 0.0);

		vec3 lightPosView = vec3(camMatrix * vec4(lightPosition, 1.0));

		//Diffuse
		vec3 viewDir = normalize(-position);
		vec3 lightDir = normalize(lightPosView - position);
		vec3 diffuse = max(dot(normal, lightDir), 0.0) *
			albedoCol * data[lightIndex].lightColour.rgb;

		//Specular
		vec3 halfDir = normalize(lightDir + viewDir);
		float specPower = pow(max(dot(normal, halfDir), 0.0), 500.0);
		vec3 specular = data[lightIndex].lightColour.rgb * specPower;

		//Attenuation
		float dist = length(lightPosView - position);
		float attenuation = 1.0 - clamp(dist / data[lightIndex].lightRadius, 0.0, 1.0);
		attenuation *= data[lightIndex].intensity;

		if (lightIndex < numShadowCastingLights)
		{
			//Shadow
			vec4 shadowProj = (texMatrices[lightIndex] * inverse(camMatrix) *
				vec4(position + (normal * 1.5), 1));

			float lambert = max(0.0, dot(lightDir, normal));
			float shadow = 1.0;

			if (shadowProj.w > 0.0)
			{
				shadow = textureProj(shadows[lightIndex], shadowProj);
			}

			lambert *= shadow;
			attenuation *= lambert;
		}

		diffuse *= attenuation;
		specular *= attenuation;

		colour += (diffuse + specular);

		lightResult += colour;
	}

	//Ambient
	float ambientFX = 0.5;// *albedoCol;

	for (int j = 0; j < 1; j++) 
	{
		ambientFX *= texture(ambientTextures[j], TexCoords).r;
	}

	//vec3 ambient = vec3(0.5 * albedoCol * texture(ambientTextures[0], TexCoords).r);
	vec3 ambient = vec3(albedoCol * ambientFX);
	lightResult += ambient;

	FragColor = vec4(lightResult, 1.0);

	vec3 greyscale = vec3(0.2126, 0.7152, 0.0722);
	float brightness = dot(FragColor.rgb, greyscale);
	if (brightness > 0.7) {
		BrightnessCol = vec4(FragColor.rgb * vec3(1, 0.6, 0.6), 1.0);
	}
	else BrightnessCol = vec4(0.0, 0.0, 0.0, 1.0);

	//gl_FragColor.rgb = vec3(texture(ambientTextures[0], TexCoords).r, 
	//	texture(ambientTextures[0], TexCoords).r,
	//	texture(ambientTextures[0], TexCoords).r);
}