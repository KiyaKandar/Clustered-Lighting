#version 430

uniform sampler2D scene;
uniform sampler2D gPosition;

uniform mat4 projMtx;
uniform mat4 transformEyeSpace;

uniform int fps;

in vec2 TexCoords;
out vec4 FragColor;

const int NUM_SAMPLES = 200;
const int TARGET_FPS = 59;

void main(void)
{
	vec3 worldPos = texture(gPosition, TexCoords).rgb;
	vec3 initialColour = texture2D(scene, TexCoords).rgb;
	vec3 origColour = initialColour;

	if (worldPos.z > 0.0f) 
	{
		FragColor = vec4(origColour, 1.0f);
	}
	else
	{
		vec4 currentPos = vec4((TexCoords.x * 2) - 1, ((TexCoords.y) * 2) - 1,
			0, 1);

		vec3 newEyeSpace = worldPos;
		vec3 oldEyeSpace = (transformEyeSpace * vec4(worldPos, 1.0f)).xyz;

		vec4 newSS = projMtx * vec4(newEyeSpace, 1.0f);
		vec4 oldSS = projMtx * vec4(oldEyeSpace, 1.0f);
		newSS.xyz = newSS.xyz / newSS.w;
		oldSS.xyz = oldSS.xyz / oldSS.w;

		vec2 velocity = ((oldSS - newSS) / (1.8f *  NUM_SAMPLES)).xy;
		velocity = velocity * (fps / TARGET_FPS);

		float alpha = texture2D(scene, TexCoords).a;
		vec2 velocityTexCoord = TexCoords;

		velocityTexCoord += velocity;

		for (int i = 0; i < NUM_SAMPLES; ++i)
		{
			vec2 offset = velocity * (float(i) / float(NUM_SAMPLES - 1) - 0.5);

			initialColour += texture2D(scene, velocityTexCoord + offset).rgb;
			velocityTexCoord += velocity;
		}

		FragColor = vec4(mix(origColour, initialColour / NUM_SAMPLES, 1), texture2D(scene, TexCoords).a);//vec4(origColour, 1.0f);//
	}
}