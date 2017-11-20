#version 330 core

uniform sampler2D 	diffuseTex1;
uniform sampler2D	diffuseTex2;

uniform float time;
uniform float alphaDecay;

in Vertex	{	
	vec2 texCoord;
	vec4 colour;
} IN;

layout(location = 0) out vec4 FragColor;
//out vec4 gl_FragColor;

void main(void) {
	vec4 texCol1 = texture(diffuseTex1 , IN.texCoord );
	vec4 texCol2 = texture(diffuseTex2 , IN.texCoord );
	vec4 texCol = mix(texCol2, texCol1, cos(time / 200));// texCol1 * texCol2;
	
	if (texCol.a > 0.1f) 
	{
		texCol.a = texCol.a - time / 300;
		//texCol.a = texCol.a - alphaDecay;// *time / 300);
	}
	
	//if (texCol == vec4(0.0f, 0.0f, 0.0f, 0.0f))
	if (texCol.a < 0.01f)
	{
		discard;
	}

	FragColor = texCol;//texture(diffuseTex, IN.texCoord);//vec4(1.0f, 0.0f, 0.0f, 1.0f);//vec4(1.0f, 0.0f, 0.0f, 1.0f);// 
}