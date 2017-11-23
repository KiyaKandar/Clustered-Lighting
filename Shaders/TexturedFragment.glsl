#version 150
uniform sampler2D diffuseTex;

in Vertex {
	vec2 texCoord;
} IN;

out vec4 gl_FragColor;

void main(void){
	gl_FragColor = texture(diffuseTex, IN.texCoord);//vec4(1, 0, 0.0f, 1.0f);//vec4(IN.texCoord.x, IN.texCoord.y, 0.0f, 1.0f);// 
}