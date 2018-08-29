#version 430

#include ../Shaders/compute/configuration.glsl

uniform int renderTiles;
uniform float nearPlane;
uniform float farPlane;

uniform float ambientLighting;
uniform vec4  cameraPos;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D depth;

uniform mat4 texMatrices[5];
uniform mat4 camMatrix;
uniform mat4 viewMatrix;

uniform int numXTiles;
uniform int numYTiles;

in vec2 TexCoords;
in mat4 textureMat;
in vec2 screenPos;

out vec4 FragColor;

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
	LightData lightData[];
};

layout (std430, binding = 3) buffer TileLightsBuffer
{
	int lightIndexes[numTiles];
	int tileLights[][numLights];
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
		float specPower = pow(max(dot(normal, halfDir), 0.0), 50.0);
		vec3 specular = lightData[lightIndex].lightColour.rgb * specPower;

		float attenuation = 1.0 - clamp(dist / lightData[lightIndex].lightRadius, 0.0, 1.0);
		attenuation *= lightData[lightIndex].intensity;

		diffuse *= attenuation;
		specular *= attenuation;

		colour += (diffuse + specular);

		lightResult.rgb += colour;
	}
}

void main(void)
{
    //Retrieve data from gbuffer
    vec3 position = texture(gPosition, TexCoords).rgb;
    vec3 normal = normalize(texture(gNormal, TexCoords).rgb);
	vec4 albedoCol = texture(gAlbedo, TexCoords);

	//Transform screenspace coordinates into a tile index
	float zCoord = abs(position.z) / (farPlane - nearPlane);

	int xIndex = int(TexCoords.x * tilesOnAxes.x);
	int yIndex = int(TexCoords.y * tilesOnAxes.y);
	int zIndex =  int(zCoord * tilesOnAxes.z);

	int tile = GetTileIndex(xIndex, yIndex, zIndex);

	if (renderTiles == 0)
	{
		//Default value
		vec4 lightResult = vec4(0.0, 0.0, 0.0, 1.0);
		for (int j = 0; j < lightIndexes[tile]; j++)
		{
			int lightIndex = tileLights[tile][j];
			AddBPLighting(position, normal, albedoCol, lightIndex, lightResult);
		}

		lightResult.rgb += albedoCol.rgb * ambientLighting;
		lightResult.a = albedoCol.a;
		FragColor = lightResult;
	}
	else
	{
		float colourValue = (float(lightIndexes[tile]) / float(numLights));
		FragColor = vec4(colourValue, colourValue, colourValue, 1.0f);
	}
}