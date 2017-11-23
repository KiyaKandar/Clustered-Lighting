#version 430

layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 BrightnessCol;

#include ../Shaders/compute/configuration.glsl

uniform int numShadowCastingLights;
uniform float ambientLighting;
uniform vec3  cameraPos;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D ssao;

uniform sampler2DShadow shadows[5];
uniform mat4 texMatrices[5];
uniform mat4 camMatrix;
uniform mat4 viewMatrix;

uniform sampler2D ambientTextures[1];

uniform int numXTiles;
uniform int numYTiles;

in vec2 TexCoords;
in mat4 textureMat;
in vec2 screenPos;

const int HALF_NUM_PCF_SAMPLES = 2;

//out vec4 FragColor;

struct LightData
{
	vec4 pos4;
	vec4 lightColour;
	float lightRadius;
	float intensity;

	float fpadding[2];
};

struct SpotLightData
{
	vec4 direction;
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
	LightData lightData[];
};

layout (std430, binding = 3) buffer TileLightsBuffer
{
	int lightIndexes[numTiles];
	int tileLights[numTiles][numLights];
};

layout(std430, binding = 7) buffer SpotLightDataBuffer
{
	SpotLightData spotLightData[];
};

void AddBPLighting(vec3 position, vec3 normal, vec4 albedoCol, int lightIndex, inout vec4 lightResult)
{
	vec3 lightPosition = lightData[lightIndex].pos4.xyz;
	vec3 colour = vec3(0.0, 0.0, 0.0);
	vec3 lightPosView = vec3(camMatrix * vec4(lightPosition, 1.0));
	float dist = length(lightPosView - position);

	if (dist <= lightData[lightIndex].lightRadius)
	{
		//Diffuse
		vec3 viewDir = normalize(-position);
		vec3 lightDir = normalize(lightPosView - position);
		vec3 diffuse = max(dot(normal, lightDir), 0.0) *
			albedoCol.rgb * lightData[lightIndex].lightColour.rgb;

		//Specular
		vec3 halfDir = normalize(lightDir + viewDir);
		float specPower = pow(max(dot(normal, halfDir), 0.0), 500.0);
		vec3 specular = lightData[lightIndex].lightColour.rgb * specPower;

		//Attenuation
		float attenuation = 0;

		if (spotLightData[lightIndex].direction != vec4(0, 0, 0, 0)) {
			vec3 spotLightPosView = ((mat3(camMatrix) * spotLightData[lightIndex].direction.xyz)).xyz;
			vec3 spotLightDir = normalize(position - lightPosView);

			float theta = dot(spotLightDir, normalize(spotLightPosView));

			float coneAngle = radians(spotLightData[lightIndex].direction.w);
			if (theta > cos(coneAngle))
			{
				attenuation = 1.0 - clamp(dist / lightData[lightIndex].lightRadius, 0.0, 1.0);
				attenuation *= lightData[lightIndex].intensity;
			}
		}
		else
		{
			attenuation = 1.0 - clamp(dist / lightData[lightIndex].lightRadius, 0.0, 1.0);
			attenuation *= lightData[lightIndex].intensity;
		}

		if (lightIndex < numShadowCastingLights)
		{
			float lambert = max(0.0, dot(lightDir, normal));

			//Shadow
			vec4 shadowProj = (texMatrices[lightIndex] * inverse(camMatrix) *
				vec4(position + (normal * 1.5), 1));

			float shadow = 0.0;

			if (shadowProj.w > 0.0)
			{
				vec2 texelSize = 10.0 / textureSize(shadows[lightIndex], 0);

				for (int x = -HALF_NUM_PCF_SAMPLES; x < HALF_NUM_PCF_SAMPLES; ++x)
				{
					for (int y = -HALF_NUM_PCF_SAMPLES; y < HALF_NUM_PCF_SAMPLES; ++y)
					{
						vec2 sampleCoord = vec2(x, y) *texelSize;
						shadow += textureProj(shadows[lightIndex], shadowProj + vec4(sampleCoord, 0.0f, 0.0f));
					}
				}

				shadow /= 16;// pow((HALF_NUM_PCF_SAMPLES) * 2, 2);
				//shadow /= 16;
				//shadow += textureProj(shadows[lightIndex], shadowProj);
			}

			lambert *= shadow;
			attenuation *= lambert;
		}

		diffuse *= attenuation;
		specular *= attenuation;

		colour += (diffuse + specular);

		lightResult.rgb += colour;
		//lightResult.a = albedoCol.a;
	}

	
}

void main(void){
    //Retrieve data from gbuffer
    vec3 position	= texture(gPosition, TexCoords).rgb;
    vec3 normal		= normalize(texture(gNormal, TexCoords).rgb);
	vec4 albedoCol = texture(gAlbedo, TexCoords); //vec3(0.75f, 0.75f, 0.75f);// vec3(0.1, 0.1, 0.1);//

	if (position.z > 0.0f) 
	{
		FragColor = albedoCol;
	}
	else 
	{
		//Transform screenspace coordinates into a tile index
		float xCoord = gl_FragCoord.x / 1280;
		float yCoord = gl_FragCoord.y / 720;
		float zCoord = gl_FragCoord.z;

		zCoord = abs(zCoord);

		int xIndex = int(xCoord * tilesOnAxes.x);
		int yIndex = int(yCoord * tilesOnAxes.y);
		int zIndex = int(zCoord * tilesOnAxes.z);

		int tile = 0;// xIndex + (yIndex * int(tilesOnAxes.y)) + (zIndex * (int(tilesOnAxes.x * tilesOnAxes.z)));

		//Default value
		vec4 lightResult = vec4(0.0, 0.0, 0.0, 1.0);

		for(int j = 0; j < lightIndexes[tile]; j++)
		{
			int lightIndex = tileLights[tile][j];

			AddBPLighting(position, normal, albedoCol, lightIndex, lightResult);
		}

		//Ambient
		float ambientFX = ambientLighting;//0.5f;

		for (int j = 0; j < 1; j++) 
		{
			ambientFX *= texture(ambientTextures[j], TexCoords).r;
		}

		vec3 newCol = albedoCol.rgb * ambientFX; //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		vec3 ambient = newCol;// albedoCol.a);
		lightResult.rgb += ambient;
		lightResult.a = albedoCol.a;

		FragColor = lightResult;//vec4(lightResult, 1.0);
	}

	//FragColor = vec4(position.xyz, 1.0f);

	vec3 greyscale = vec3(0.2126, 0.7152, 0.0722);
	float brightness = dot(FragColor.rgb, greyscale);
	if (brightness > 0.8) 
	{
		BrightnessCol = vec4(FragColor.rgb * vec3(1, 0.6, 0.6), 1.0f/*albedoCol.a*/);
	}
	else BrightnessCol = vec4(0.0, 0.0, 0.0, 1.0f/*albedoCol.a*/);
}