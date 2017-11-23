#version 430

layout(location = 0) out vec4 FragColor;

uniform sampler2D scene;
uniform sampler2D bloomBlur;

const float exposure = 1.0f;
const float gamma = 1.2;

in vec2 TexCoords;

void main(void){
	vec4 finalColour = texture2D(scene, TexCoords);
	vec4 bloomColour = texture2D(bloomBlur, TexCoords);

	finalColour.rgb += bloomColour.rgb;

	//float alpha = 0.0f;

	//if (finalColour.a < bloomColour.a)
	//{
	//	alpha = finalColour.a;
	//}
	//else
	//{
	//	alpha = bloomColour.a;
	//}

	vec3 result = vec3(1.0f) - exp(-finalColour.rgb * exposure);
	result = pow(result, vec3(1.0f / gamma));

	FragColor = vec4(result, finalColour.a);
}