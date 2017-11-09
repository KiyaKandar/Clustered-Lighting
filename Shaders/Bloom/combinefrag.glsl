#version 430

layout(location = 0) out vec4 FragColor;

uniform sampler2D scene;
uniform sampler2D bloomBlur;

in vec2 TexCoords;
//out vec4 FragColor;

void main(void){
	const float gamma = 1;

	vec3 finalColour = texture2D(scene, TexCoords).rgb;
	vec3 bloomColour = texture2D(bloomBlur, TexCoords).rgb;

	finalColour += bloomColour;

	FragColor = vec4(finalColour, 1.0f);
}