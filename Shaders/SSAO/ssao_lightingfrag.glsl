#version 430

layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 BrightnessCol;

#include ../Shaders/compute/configuration.glsl

const float PI = 3.14159265359;

uniform int numShadowCastingLights;
uniform float ambientLighting;
uniform vec4  cameraPos;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D gMetallic;
uniform sampler2D gRoughness;

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
		float d = max(distance - lightData[lightIndex].lightRadius, 0.0f);
		lengthPos /= distance;
		float attenDenom = (d / lightData[lightIndex].lightRadius) + 1.0f;
		float dotP = max(dot(lengthPos, N), 0);
		float attenuation = 1.0f / (attenDenom * attenDenom);
		attenuation *= dotP;
		attenuation *= pow(lightData[lightIndex].intensity, 2.2f);

		if (lightIndex < numShadowCastingLights)
		{
			float lambert = max(0.0, dot(L, N));

			//Shadow
			vec4 shadowProj = (texMatrices[lightIndex] * inverse(camMatrix) *
				vec4(position + (N * 1.5), 1));

			float shadow = 0.0;

			if (shadowProj.w > 0.0)
			{
				//shadow = textureProj(shadows[lightIndex], shadowProj);

				float texelSize = 1.0f / 4096.0f;// textureSize(shadows[lightIndex], 0);
				int sampleCount = 0;

				for (int x = -2; x <= 2; ++x)
				{
					for (int y = -2; y <= 2; ++y)
					{
						vec2 sampleCoord = vec2(x, y);// *texelSize;// *100.0f;
						shadow += textureProj(shadows[lightIndex], shadowProj + vec4(sampleCoord, 0.0f, 0.0f));
						sampleCount++;
					}
				}

				shadow /= 16;// pow((HALF_NUM_PCF_SAMPLES) * 2, 2);
			}

			lambert *= shadow;
			attenuation *= lambert;
		}

		//attenuation = (attenuation - )


		////float attenuation = /*(lightData[lightIndex].lightRadius * lightData[lightIndex].lightRadius) */1.0f / (distance);
		//float attenuation = 1.0f / (distance * distance);// clamp(pow(1.0f - (pow(distance / lightData[lightIndex].lightRadius, 4.0f)), 2.0f), 0.0f, 1.0f) / (distance * distance) + 1.0f;
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

	//Ambient lighting + SSAO
	vec3 ambient = vec3(0.5) * albedo;

	//Final colour
	vec3 color = ambient + Lo;
	//color = color / (color + vec3(1.0));
	color = pow(color, vec3(1.0 / 1.3));

	lightResult = vec4(color, 1.0)  *texture(ambientTextures[0], TexCoords).r;
}

void main(void){
    //Retrieve data from gbuffer
    vec3 position	= texture(gPosition, TexCoords).rgb;
    vec3 normal		= normalize(texture(gNormal, TexCoords).rgb);
	vec4 albedoCol = texture(gAlbedo, TexCoords);

	vec3 worldPos = (inverse(camMatrix) * vec4(position, 1.0f)).xyz;

	if (position.z > 0.0f) 
	{
		//Its the skybox, dont touch it...
		FragColor = albedoCol;
	}
	else 
	{
		//Transform screenspace coordinates into a tile index
		float xCoord = gl_FragCoord.x / 1280;
		float yCoord = gl_FragCoord.y / 720;
		float zCoord = position.z;

		//zCoord = abs(zCoord);

		int xIndex = int(xCoord * tilesOnAxes.x);
		int yIndex = int(yCoord * tilesOnAxes.y);
		int zIndex = int(zCoord * tilesOnAxes.z);

		int tile = xIndex + (yIndex * int(tilesOnAxes.x)) + (zIndex * (int(tilesOnAxes.x * tilesOnAxes.y)));

		tile = 0;
		vec4 lightResult = vec4(0.0, 0.0, 0.0, 1.0);

		AddPBRLighting(position, albedoCol.rgb, normal, tile, lightResult);

		lightResult.a = albedoCol.a;
		FragColor = lightResult;
	}

	vec3 greyscale = vec3(0.2126, 0.7152, 0.0722);
	float brightness = dot(FragColor.rgb, greyscale);
	if (brightness > 0.999) 
	{
		BrightnessCol = vec4(FragColor.rgb * vec3(1, 0.5, 0.5), 1.0f);
	}
	else BrightnessCol = vec4(0.0, 0.0, 0.0, 1.0f);
}