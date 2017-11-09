#pragma once
/*
	AUTHOR: RICH DAVISON
*/
#include "../Maths/Vector4.h"
#include "../Maths/Vector3.h"

#include <algorithm>

struct LightData
{
	Vector4 lightPosition;
	Vector4 lightColour;
	float lightRadius;
	float intensity;

	float padding[2];
};

struct SpotLightData
{
	Vector4 direction;
};

class Light
{
public:
	Light(Vector3 position, Vector4 colour, float radius, float intensity, Vector4 direction = Vector4(0, 0, 0, 0)) {
		this->position	= position;
		this->colour	= colour;
		this->radius	= radius;
		this->direction = direction;

		//float positionData[3] = { position.x, position.y, position.z };
		//float colourData[4] = { colour.x, colour.y, colour.z, colour.w };
		//memcpy(data.lightPosition, positionData, 3 * sizeof(float));
		//memcpy(data.lightColour, colourData, 4 * sizeof(float));

		data.lightPosition = Vector4(position.x, position.y, position.z, 1.0f);
		data.lightColour = colour;
		data.lightRadius = radius;
		data.intensity	 = intensity;
		spotLightData.direction = direction;
	}

	Light() {
		this->position	= Vector3(0, 0, 0);
		this->colour	= Vector4(1, 1, 1, 1);
		this->radius	= 0.0f;
		this->direction = Vector4(0, 0, 0, 0);
	}

	Light(const Light& rhs)
	{
		position = rhs.position;
		colour = rhs.colour;
		radius = rhs.radius;
		direction = rhs.direction;

		data.lightPosition = Vector4(position.x, position.y, position.z, 1.0f);
		data.lightColour = colour;
		data.lightRadius = radius;
		data.intensity = rhs.data.intensity;
		spotLightData.direction = rhs.direction;
	}

	~Light(void) {};

	static void AddLightsToArray(Light** lights, int numberExistingLights, int numberOfLights, Light lightToCopy);

	Vector3 GetPosition() const			{ return position; }
	void	SetPosition(Vector3 val)	{ position = val; }

	float	GetRadius() const			{ return radius; }
	void	SetRadius(float val)		{ radius = val; }

	Vector4 GetColour() const			{ return colour; }
	void	SetColour(Vector4 val)		{ colour = val; }

	LightData GetData()
	{
		return data;
	}

	SpotLightData GetSpotData()
	{
		return spotLightData;
	}

protected:
	LightData data;
	SpotLightData spotLightData;
	Vector3 position;
	Vector4 colour;
	Vector4 direction;
	float radius;
};

