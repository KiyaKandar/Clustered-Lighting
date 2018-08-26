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
	float bulbRadius;
	float lightCutoffRadius;
	float intensity;

	float padding;
};

struct SpotLightData
{
	Vector4 direction;
};

class Light
{
public:
	Light(Vector3 position, Vector4 colour, float bulbRadius, float cutoffRadiusPercentageExtra, float intensity, Vector4 direction = Vector4(0, 0, 0, 0)) {
		this->position	= position;
		this->colour	= colour;
		this->bulbRadius = bulbRadius;
		this->cutoffRadius = CalculateCutoffRadiusAsPercentageOfBulbRadius(cutoffRadiusPercentageExtra);
		this->direction = direction;

		//float positionData[3] = { position.x, position.y, position.z };
		//float colourData[4] = { colour.x, colour.y, colour.z, colour.w };
		//memcpy(data.lightPosition, positionData, 3 * sizeof(float));
		//memcpy(data.lightColour, colourData, 4 * sizeof(float));

		data.lightPosition = Vector4(position.x, position.y, position.z, 1.0f);
		data.lightColour = colour;
		data.bulbRadius = bulbRadius;
		data.lightCutoffRadius = CalculateCutoffRadiusAsPercentageOfBulbRadius(cutoffRadiusPercentageExtra);
		data.intensity	 = intensity;
		spotLightData.direction = direction;
	}

	Light() {
		this->position	= Vector3(0, 0, 0);
		this->colour	= Vector4(1, 1, 1, 1);
		this->bulbRadius = 0.0f;
		this->cutoffRadius = 0.0f;
		this->direction = Vector4(0, 0, 0, 0);
	}

	Light(const Light& rhs)
	{
		position = rhs.position;
		colour = rhs.colour;
		bulbRadius = rhs.bulbRadius;
		cutoffRadius = rhs.cutoffRadius;
		direction = rhs.direction;

		data.lightPosition = Vector4(position.x, position.y, position.z, 1.0f);
		data.lightColour = colour;
		data.bulbRadius = bulbRadius;
		data.lightCutoffRadius = cutoffRadius;
		data.intensity = rhs.data.intensity;
		spotLightData.direction = rhs.direction;
	}

	~Light(void) {};

	static void AddLightsToArray(Light** lights, int numberExistingLights, int numberOfLights, Light lightToCopy);

	Vector3 GetPosition() const			{ return position; }
	void	SetPosition(Vector3 val)
	{
		position = val;
		data.lightPosition = Vector4(val.x, val.y, val.z, 1.0f);
	}

	float	GetRadius() const			{ return bulbRadius; }
	void	SetRadius(float val)		{ bulbRadius = val; }

	Vector4 GetColour() const			{ return colour; }
	void	SetColour(Vector4 val)
	{
		colour = val;
		data.lightColour = val;
	}

	void SetDirection(Vector4 val)
	{
		spotLightData.direction = val;
	}

	LightData GetData()
	{
		return data;
	}

	SpotLightData GetSpotData()
	{
		return spotLightData;
	}

	float CalculateCutoffRadiusAsPercentageOfBulbRadius(const float percentage)
	{
		return bulbRadius * percentage;
	}

protected:
	LightData data;
	SpotLightData spotLightData;
	Vector3 position;
	Vector4 colour;
	Vector4 direction;
	float bulbRadius;
	float cutoffRadius;
};

