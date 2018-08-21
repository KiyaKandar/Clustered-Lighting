#version 330 core

uniform mat4 projMatrix;
uniform vec3 samples[64];

int kernelSize = 64;
uniform float radius;
uniform float bias;

uniform float xSize;
uniform float ySize;

in vec2 TexCoords;

out float gl_FragColor;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D texNoise;

void main(void){
	//Generate noise scale based on screen resolution
	vec2 noiseScale = vec2(xSize / 4.0, ySize / 4.0);

	float b = 0.1;
	float r = 140.0;

	//Get input for SSAO algorithm
    vec3 fragPos = texture(gPosition, TexCoords).xyz;
    vec3 normal = texture(gNormal, TexCoords).rgb;
    vec3 randomVec = texture(texNoise, TexCoords * noiseScale).xyz;

    //Create TBN change-of-basis matrix: from tangent-space to view-space
    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);

    //Iterate over the sample kernel and calculate occlusion factor
    float occlusion = 0.0;
    for(int i = 0; i < kernelSize; ++i)
    {
        //Get sample position
        vec3 samplevec = TBN * samples[i]; // from tangent to view-space
		samplevec = fragPos + samplevec * r;
        
        //Project sample position (to sample texture) (to get position on screen/texture)
        vec4 offset = vec4(samplevec, 1.0);
        offset = projMatrix * offset; // from view to clip-space
        offset.xyz /= offset.w; // perspective divide
        offset.xyz = offset.xyz * 0.5 + 0.5; // transform to range 0.0 - 1.0
        
        //Get sample depth
        float sampleDepth = texture(gPosition, offset.xy).z; // get depth value of kernel sample
        
        //Range check & accumulate
        float rangeCheck = smoothstep(0.0, 1.0, r / abs(fragPos.z - sampleDepth));
		occlusion += (sampleDepth >= samplevec.z + b ? 1.0 : 0.0) * rangeCheck;
    }

    occlusion = 1.0 - (occlusion / kernelSize);
    
    gl_FragColor = pow(occlusion, 2.5);
}