#version 330 core

uniform mat4 modelMatrix ;
uniform mat4 viewMatrix ;
uniform mat4 projMatrix ;

uniform float time;

layout(location = 0) in vec3 position;
//layout(location = 1) in vec2 texCoord;

out Vertex {
	//vec2 texCoord;
	vec4 colour;
} OUT;

void main(void)	{
	vec3 newPosition = position;
	newPosition.x = newPosition.x * cos(time);
	newPosition.y = newPosition.y * cos(time);
	newPosition.z = newPosition.z * cos(time);
	
	vec3 pos = vec3(0, 500, 0);

	vec4 worldPos = modelMatrix * vec4(pos, 1.0);

	vec4 position = (projMatrix * viewMatrix) * worldPos;
	gl_Position = position.xyzw;//vec4(position, 1.0); 
	//OUT.texCoord = texCoord;
	OUT.colour = vec4(0.0f, 0.0f, 1.0f, 1.0f);// colour;
}