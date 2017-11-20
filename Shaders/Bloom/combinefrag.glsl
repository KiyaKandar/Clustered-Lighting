#version 430

layout(location = 0) out vec4 FragColor;

uniform sampler2D scene;
uniform sampler2D bloomBlur;

in vec2 TexCoords;

void main(void){
	const float gamma = 1;

	vec4 finalColour = texture2D(scene, TexCoords);
	vec4 bloomColour = texture2D(bloomBlur, TexCoords);

	finalColour.rgb += bloomColour.rgb;

	float alpha = 0.0f;

	if (finalColour.a < bloomColour.a)
	{
		alpha = finalColour.a;
	}
	else
	{
		alpha = bloomColour.a;
	}

	FragColor = vec4(finalColour.rgb, alpha);
}