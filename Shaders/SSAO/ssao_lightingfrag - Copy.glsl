#version 330 core

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

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D ssao;

in vec2 TexCoords;
//in vec4 shadowProj;

out vec4 gl_FragColor;

void main(void){
    // retrieve data from gbuffer
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 normal = texture(gNormal, TexCoords).rgb;
    vec3 Diffuse = texture(gAlbedo, TexCoords).rgb;
    float AmbientOcclusion = texture(ssao, TexCoords).r;

	vec3 ambient = vec3(0.3 * Diffuse * AmbientOcclusion);
	vec3 lighting = ambient;

	vec3 incident = normalize(lightPos - FragPos);
	float lambert = max(0.0, dot(incident, normal));
	
	float dist    = length(lightPos - FragPos);
	float atten   = 1.0 - clamp(dist / lightRadius, 0.0, 1.0);

	vec3 lightDir = normalize(lightPos - FragPos);
	vec3 diffuse = max(dot( normal, lightDir), 0.0) * Diffuse * lightColour.rgb;

	vec3 viewDir  = normalize(-FragPos);
	vec3 halfDir  = normalize(lightDir + viewDir);

	vec4 specularVal = texture2D(texture_specular, TexCoords);
	float spec = pow(max(dot(normal, halfDir), 0.0), 8.0);

	float rFactor = max(0.0, dot(halfDir, normal));
	float sFactor = pow(rFactor, 10.0);

	float shadow  = 1.0;
	
	//if(shadowProj.w > 0.0) {
		//shadow = textureProj(shadowTex, shadowProj);
	//}

	//lambert *= shadow;

	vec3 specular = lightColour.rgb * spec;

	vec3 colour   = (diffuse.rgb * lightColour.rgb);
	colour       += (lightColour.rgb * sFactor) * specularVal.rgb;

	diffuse *= atten;
	//diffuse *= lambert;
	//vec3 spec = lightColour.rgb * sFactor;
	//spec += atten;
	specular *= atten;
	lighting += diffuse + specular;

	//vec3 lighting = ambient;
	//lighting += diffuse + spec;

	gl_FragColor = vec4(lighting, 1.0);

	//gl_FragColor  = vec4(colour * atten * lambert, diffuse.a);
	//gl_FragColor.rgb += (diffuse.rgb * lightColour.rgb) * ambient;
}