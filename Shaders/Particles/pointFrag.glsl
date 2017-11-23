#version 330 core

uniform sampler2D 	diffuseTex1;
uniform sampler2D	diffuseTex2;

uniform float time;
uniform float alphaDecay;
uniform vec4 colour;

in Vertex	{	
	vec2 texCoord;
	vec4 colour;
} IN;

layout(location = 0) out vec4 FragColor;
//out vec4 gl_FragColor;

void main(void) {
	vec4 fragColor = colour;
	vec2 origin = vec2(0.5, 0.5);
	vec2 distance = origin - IN.texCoord;
	distance = abs(distance);
	float length = length(distance);

	if (fragColor.a > 0.01f)
	{
		fragColor.a = fragColor.a - ((length) * 2) - alphaDecay;
		//texCol.a = texCol.a - alphaDecay;// *time / 300);
	}
	
	//if (texCol < vec4(0.01f, 0.01f, 0.01f, 0.01f))
	if (fragColor.a < 0.01f)
	{
		discard;
	}

	FragColor = fragColor;//texture(diffuseTex, IN.texCoord);//vec4(1.0f, 0.0f, 0.0f, 1.0f);//vec4(1.0f, 0.0f, 0.0f, 1.0f);// 
}