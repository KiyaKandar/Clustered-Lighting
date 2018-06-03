#version 430

uniform sampler2D image;
uniform bool horizontal;

uniform float weight[5] = float[](0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162);
/*
(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.000003, 0.000229, 0.005977, 0.060598,
0.24173, 0.382925, 0.24173, 0.060598, 0.005977, 0.000229, 0.000003, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
);//
*/

in vec2 TexCoords;
out vec4 gl_FragColor;

void main(void){
	vec2 tex_offset = 1.0 / textureSize(image, 0); // gets size of single texel
	vec3 result = texture(image, TexCoords).rgb * weight[0];

	if (horizontal)
	{
		for (int i = 1; i < 5; ++i)
		{
			result += texture(image, TexCoords + vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
			result += texture(image, TexCoords - vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
		}
	}
	else
	{
		for (int i = 1; i < 5; ++i)
		{
			result += texture(image, TexCoords + vec2(0.0, tex_offset.y * i)).rgb * weight[i];
			result += texture(image, TexCoords - vec2(0.0, tex_offset.y * i)).rgb * weight[i];
		}
	}

	//for (int x = -xSize; x < xSize; ++x)
	//{
	//	for (int y = -ySize; y < ySize; ++y)
	//	{
	//		vec2 offset = vec2(float(x), float(y)) * texelSize;
	//		result += texture(image, TexCoords + offset).rgb;
	//	}
	//}

	gl_FragColor = vec4(result, texture(image, TexCoords).a);
}