#version 330 core;

uniform float exposure;
uniform float decay;
uniform float density;
uniform float weight;

uniform vec2 lightPositionOnScreen;
uniform sampler2D myTexture;

const int NUM_SAMPLES = 100;

void main()
{	
	vec2 deltaTextCoord = vec2( gl_TexCoord[0].st - lightPositionOnScreen.xy );
	vec2 textCoo = gl_TexCoord[0].st;
	deltaTextCoord *= 1.0 /  float(NUM_SAMPLES) * density;
	float illuminationDecay = 1.0;
	
	
	for(int i=0; i < NUM_SAMPLES ; i++)
	{
			textCoo -= deltaTextCoord;
			vec4 sample = texture2D(myTexture, textCoo );
			
			sample *= illuminationDecay * weight;
			
			gl_FragColor += sample;
			
			illuminationDecay *= decay;
	}
	
	gl_FragColor *= exposure;
}
