#version 330 core

out float gl_FragColor;

in vec2 TexCoords;

uniform sampler2D ssaoInput;
uniform int xSize;
uniform int ySize;

void main(void){

	vec2 texelSize = 1.0 / vec2(textureSize(ssaoInput, 0));
	texelSize *= 1;
    float result = 0.0;

    for (int x = -xSize; x < xSize; ++x)
    {
        for (int y = -ySize; y < ySize; ++y)
        {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            result += texture(ssaoInput, TexCoords + offset).r;
        }
    }

	gl_FragColor = result / ((xSize * 2) * (ySize * 2)); //texture(ssaoInput, TexCoords);
}