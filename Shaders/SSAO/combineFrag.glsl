#version 150 core

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D ssaoColorBufferBlur;

in Vertex {
	vec2 texCoord;
} IN;

out vec4 gl_FragColor;

void main(void){
	vec3 position  = texture(gPosition, IN.texCoord).xyz;
	vec3 normal = texture(gNormal, IN.texCoord).xyz;
	vec3 albedo = texture(gAlbedo, IN.texCoord).xyz;
	vec3 ssao = texture(ssaoColorBufferBlur, IN.texCoord).xyz;
	
	gl_FragColor.xyz = position; 
	gl_FragColor.xyz += normal; 
	gl_FragColor.xyz += albedo;
	gl_FragColor.xyz += ssao;
	gl_FragColor.a = 1.0;
}