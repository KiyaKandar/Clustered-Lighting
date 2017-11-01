#include "Light.h"

void Light::AddLightsToArray(Light ** lights, int numberExistingLights, 
	int numberOfLights, Light lightToCopy)
{
	for (int i = numberExistingLights; i < numberOfLights; i++)
	{
		lights[i] = new Light(lightToCopy);
	}
}
