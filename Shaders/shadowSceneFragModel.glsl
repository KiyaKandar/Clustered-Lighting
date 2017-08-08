#version 150 core

uniform int useTexture;
uniform int useBump;

uniform sampler2D texture_diffuse;
uniform sampler2D bumpTex;
uniform sampler2DShadow shadowTex;

uniform sampler2D texture_specular;
uniform sampler2D texture_height;

uniform vec3  cameraPos;
uniform vec4  lightColour;
uniform vec3  lightPos;
uniform float lightRadius;

in Vertex {
	vec2 texCoord;
	vec3 normal;
	vec3 tangent;
	vec3 binormal;
	vec3 worldPos;
	vec4 shadowProj;
	mat3 normalMatrix;
} IN;

out vec4 gl_FragColor;

void main(void){
	vec4 diffuse = texture2D(texture_diffuse, IN.texCoord);

	//mat3 TBN = mat3(IN.tangent, IN.binormal, IN.normal);

	vec3 normal;
	if(useBump == 1) {
		vec3 nNormal		= normalize(IN.normalMatrix * normalize(IN.normal));
		vec3 nTangent	= normalize(IN.normalMatrix * normalize(IN.tangent));
		vec3 nBinormal	= normalize(IN.normalMatrix * normalize(cross(IN.normal, IN.tangent)));
		mat3 TBN = mat3(nTangent, nBinormal, nNormal);

		normal = normalize(TBN * (texture(texture_height, IN.texCoord).rgb * 2.0 - 1.0));
	}
	else normal = IN.normal;

	vec3 incident = normalize(lightPos - IN.worldPos);
	float lambert = max(0.0, dot(incident, normal));
	
	float dist    = length(lightPos - IN.worldPos);
	float atten   = 1.0 - clamp(dist / lightRadius, 0.0, 1.0);

	vec3 viewDir  = normalize(cameraPos - IN.worldPos);
	vec3 halfDir  = normalize(incident + viewDir);

	vec4 specularVal = texture2D(texture_specular, IN.texCoord);

	float rFactor = max(0.0, dot(halfDir, normal));
	float sFactor = pow(rFactor, 300.0);

	float shadow  = 1.0;
	
	if(IN.shadowProj.w > 0.0) {
		shadow = textureProj(shadowTex, IN.shadowProj);
	}

	lambert *= shadow;

	vec3 colour   = (diffuse.rgb * lightColour.rgb);
	colour       += (lightColour.rgb * sFactor) * specularVal.rgb;

	gl_FragColor  = vec4(colour * atten * lambert, diffuse.a);
	gl_FragColor.rgb += (diffuse.rgb * lightColour.rgb) * 0.5;
}