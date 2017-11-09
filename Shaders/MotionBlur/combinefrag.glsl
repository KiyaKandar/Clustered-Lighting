#version 430

uniform sampler2D scene;
uniform sampler2D gPosition;

uniform mat4 projMtx;
uniform mat4 transformEyeSpace;

uniform int fps;

in vec2 TexCoords;
out vec4 FragColor;

const int NUM_SAMPLES = 700;
const int TARGET_FPS = 60;

void main(void)
{
	vec4 currentPos = vec4((TexCoords.x * 2) - 1, ((TexCoords.y) * 2) - 1,
		0, 1);

	vec3 worldPos = texture(gPosition, TexCoords).rgb;

	vec3 newEyeSpace = worldPos;
	vec3 oldEyeSpace = (transformEyeSpace * vec4(worldPos, 1.0f)).xyz;

	vec4 newSS = projMtx * vec4(newEyeSpace, 1.0f);
	vec4 oldSS = projMtx * vec4(oldEyeSpace, 1.0f);
	newSS.xyz = newSS.xyz / newSS.w;
	oldSS.xyz = oldSS.xyz / oldSS.w;

	vec2 velocity = ((oldSS - newSS) / (1.6f *  NUM_SAMPLES)).xy; 
	velocity = velocity * (fps / TARGET_FPS);

	vec3 initialColour = texture2D(scene, TexCoords).rgb;
	vec3 origColour = initialColour;
	vec2 velocityTexCoord = TexCoords;

	velocityTexCoord += velocity;

	for (int i = 0; i < NUM_SAMPLES; ++i)
	{
		vec2 offset = velocity * (float(i) / float(NUM_SAMPLES - 1) - 0.5);

		initialColour += texture2D(scene, velocityTexCoord + offset).rgb;
		velocityTexCoord += velocity;
	}

	FragColor = vec4(mix(origColour, initialColour / NUM_SAMPLES, 0.5), 1.0f);
}