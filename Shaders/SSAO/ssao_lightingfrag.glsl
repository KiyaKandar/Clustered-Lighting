#version 430

#include ../Shaders/compute/configuration.glsl

uniform int renderTiles;
uniform float nearPlane;
uniform float farPlane;

const float PI = 3.14159265359;

uniform float ambientLighting;
uniform vec4  cameraPos;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D gMetallic;
uniform sampler2D gRoughness;

uniform sampler2D depth;

uniform mat4 texMatrices[5];
uniform mat4 camMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

uniform int numXTiles;
uniform int numYTiles;

in vec2 TexCoords;
in vec2 screenPos;

out vec4 FragColor;

struct LightData
{
	vec4 pos4;
	vec4 lightColour;
	float bulbRadius;
	float lightCutoffRadius;
	float intensity;

	float fpadding;
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

vec3 getNormalFromMap(vec3 worldPos, vec3 normal)
{
	vec3 tangentNormal = normal * 2.0f - 1.0f;

	vec3 Q1 = dFdx(worldPos);
	vec3 Q2 = dFdy(worldPos);
	vec2 st1 = dFdx(TexCoords);
	vec2 st2 = dFdy(TexCoords);

	vec3 N = normalize(normal);
	vec3 T = normalize(Q1*st2.t - Q2 * st1.t);
	vec3 B = -normalize(cross(N, T));
	mat3 TBN = mat3(T, B, N);

	return normalize(TBN * tangentNormal);
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
	float a = roughness * roughness;
	float a2 = a * a;
	float NdotH = max(dot(N, H), 0.0);
	float NdotH2 = NdotH * NdotH;

	float nom = a2;
	float denom = (NdotH2 * (a2 - 1.0) + 1.0);
	denom = PI * denom * denom;

	return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
	float r = (roughness + 1.0);
	float k = (r*r) / 8.0;

	float nom = NdotV;
	float denom = NdotV * (1.0 - k) + k;

	return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float ggx2 = GeometrySchlickGGX(NdotV, roughness);
	float ggx1 = GeometrySchlickGGX(NdotL, roughness);

	return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
	return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

float CalculateAttenuation(const int lightIndex, vec3 distanceToLight, const vec3 normal)
{
	float distance = length(distanceToLight);
	float d = max(distance - lightData[lightIndex].bulbRadius, 0.0f);
	float attenDenom = (d / lightData[lightIndex].bulbRadius) + 1.0f;
	float attenuation = 1.0f / (attenDenom * attenDenom);

	distanceToLight /= distance;
	float dotP = max(dot(distanceToLight, normal), 0);
	attenuation *= dotP;

	attenuation *= pow(lightData[lightIndex].intensity, 2.2f);

	//If the fragment is beyond the bulb radius, fade the light out
	//so it reaches attenuation = 0 by cutoff radius
	if (distance >= lightData[lightIndex].bulbRadius)
	{
		float radiusToCutoffDistance = (lightData[lightIndex].lightCutoffRadius) - lightData[lightIndex].bulbRadius;
		float portionCoveredByFragment = 1.0f - ((distance - lightData[lightIndex].bulbRadius) / radiusToCutoffDistance);
		attenuation *= portionCoveredByFragment;
	}

	return attenuation;
}

void AddPBRLighting(vec3 position, vec3 albedoCol, vec3 normal, int tileIndex, inout vec4 lightResult)
{
	float metallic = texture2D(gMetallic, TexCoords).r;// 0.0f;
	float roughness = texture2D(gRoughness, TexCoords).r;// 0.5f; //

	vec3 worldPos = (/*inverse(camMatrix) **/ vec4(position, 1.0f)).xyz;
	vec3 albedo = pow(albedoCol, vec3(1.3));

	vec4 viewCameraPos = camMatrix * vec4(cameraPos.xyz, 1.0f);

	vec3 N = normal;
	vec3 V = normalize(viewCameraPos.xyz - worldPos);

	vec3 F0 = vec3(0.0f);//
	if (metallic > 0.0f)
	{
		F0 = mix(albedo, albedo, metallic);
	}
	else
	{
		F0 = vec3(0.04f);
	}

	vec3 Lo = vec3(0.0);

	for (int i = 0; i < lightIndexes[tileIndex]; i++)
	{
		int lightIndex = tileLights[tileIndex][i];

		vec3 lightPosition = (camMatrix * vec4(lightData[lightIndex].pos4.xyz, 1.0)).xyz;

		// calculate per-light radiance
		vec3 L = normalize(lightPosition - worldPos);
		vec3 H = normalize(V + L);

		vec3 lengthPos = lightPosition - worldPos;
		float distance = length(lengthPos);

		if (distance <= lightData[lightIndex].lightCutoffRadius)
		{
			float attenuation = CalculateAttenuation(lightIndex, lengthPos, N);

			vec3 radiance = lightData[lightIndex].lightColour.rgb * attenuation;

			// cook-torrance brdf
			float NDF = DistributionGGX(N, H, roughness);
			float G = GeometrySmith(N, V, L, roughness);
			vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

			vec3 kS = F;
			vec3 kD = vec3(1.0) - kS;
			kD *= 1.0 - metallic;

			vec3 numerator = NDF * G * F;
			float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001;
			vec3 specular = numerator / denominator;

			// add to outgoing radiance Lo
			float NdotL = max(dot(N, L), 0.0);
			Lo += (kD * albedo / PI + specular) * radiance * NdotL;
		}

	}

	//Ambient lighting + SSAO
	vec3 ambient = vec3(0.11) * albedo;

	//Final colour
	vec3 color = (ambient + Lo);
	color = pow(color, vec3(1.0 / 1.3));

	lightResult = vec4(color, 1.0);
}

void main(void)
{
    //Retrieve data from gbuffer
    vec3 position = texture(gPosition, TexCoords).rgb;
    vec3 normal = normalize(texture(gNormal, TexCoords).rgb);
	vec4 albedoCol = texture(gAlbedo, TexCoords);

	//Transform screenspace coordinates into a tile index
	vec4 projPosition = projMatrix * vec4(position, 1.0f);
	projPosition.xy /= projPosition.w;
	projPosition.x = (projPosition.x + 1.0f) / 2.0f;
	projPosition.y = (projPosition.y + 1.0f) / 2.0f;

	float zCoord = abs(projPosition.z) / (farPlane - nearPlane);

	int xIndex = int(projPosition.x * (tilesOnAxes.x - 1));
	int yIndex = int(projPosition.y * (tilesOnAxes.y - 1));
	int zIndex = int(zCoord * (tilesOnAxes.z - 1));

	int tile = GetTileIndex(xIndex, yIndex, zIndex);

	if (renderTiles == 0)
	{
		//Default value
		vec4 lightResult = vec4(0.0, 0.0, 0.0, 1.0);
		AddPBRLighting(position, albedoCol.rgb, normal, tile, lightResult);

		lightResult.a = albedoCol.a;
		FragColor = lightResult;
	}
	else
	{
		float colourValue = (float(lightIndexes[tile]) / float(numLights));/* * (1.0f - zCoord)*/;
		FragColor = normalize(vec4(colourValue, colourValue, colourValue, 1.0f));
	}
}