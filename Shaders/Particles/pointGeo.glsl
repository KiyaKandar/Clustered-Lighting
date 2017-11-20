#version 330 core

uniform float particleSize;

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

in Vertex {
	//vec2 texCoord;
	vec4 colour;
} IN[];

out Vertex {
	vec2 texCoord;
	vec4 colour;
} OUT;

void main () {
	for(int i = 0; i < gl_in.length(); ++i){
		OUT.colour = IN[i].colour;
		//Top right
		gl_Position = gl_in[i].gl_Position;
		gl_Position.x += particleSize;
		gl_Position.y += particleSize;
		OUT.texCoord = vec2(1,0);
		EmitVertex();

		//Top left
		gl_Position = gl_in[i].gl_Position;
		gl_Position.x -= particleSize;
		gl_Position.y += particleSize;
		OUT.texCoord = vec2(0,0);
		EmitVertex();

		//Bottom right
		gl_Position = gl_in[i].gl_Position;
		gl_Position.x += particleSize;
		gl_Position.y -= particleSize;
		OUT.texCoord = vec2(1,1);
		EmitVertex();

		//Bottom left
		gl_Position = gl_in[i].gl_Position;
		gl_Position.x -= particleSize;
		gl_Position.y -= particleSize;
		OUT.texCoord = vec2(0,1);
		EmitVertex();
		
		EndPrimitive();
	}
}