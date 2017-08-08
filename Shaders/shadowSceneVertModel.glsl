#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;


uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat4 textureMatrix;

in vec3 position;
in vec4 colour;
in vec3 normal;
in vec3 tangent;
in vec2 texCoord;

out Vertex {
	vec2 texCoord;
	vec3 normal;
	vec3 tangent;
	vec3 binormal;
	vec3 worldPos;
	vec4 shadowProj;
	mat3 normalMatrix;
} OUT;

void main(void) {
	mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));
	OUT.normalMatrix = normalMatrix;
	OUT.texCoord	= aTexCoords;

	OUT.normal 		= aNormal;
	OUT.tangent		= aTangent;
	OUT.binormal	= aBitangent;

	//OUT.normal 		= normalize(normalMatrix * normalize(aNormal));
	//OUT.tangent		= normalize(normalMatrix * normalize(aTangent));
	//OUT.binormal	= normalize(normalMatrix * normalize(cross(aNormal, aTangent)));

	OUT.worldPos 	= (modelMatrix * vec4(position, 1)).xyz;
	OUT.shadowProj 	= (textureMatrix * vec4(position + (aNormal * 1.5), 1));

	gl_Position 	= (projMatrix * viewMatrix * modelMatrix) * vec4(position, 1.0);
}