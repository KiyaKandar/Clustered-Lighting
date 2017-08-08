#version 330 core

out float gl_FragColor;

in vec2 TexCoords;

uniform sampler2D ssaoInput;

void main(void){

	vec2 texelSize = 1.0 / vec2(textureSize(ssaoInput, 0));
	texelSize *= 1;
    float result = 0.0;
    for (int x = -2; x < 2; ++x) 
    {
        for (int y = -2; y < 2; ++y) 
        {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            result += texture(ssaoInput, TexCoords + offset).r;
        }
    }
	gl_FragColor = result / (4 * 4); //texture(ssaoInput, TexCoords);
}