/*
	AUTHOR: RICH DAVISON +
	KIYAVASH KDNAR WHERE SPECIFIED
*/
#pragma once

#include "Window.h"
#include "../../Maths/Matrix4.h"
#include "../../Maths/Vector3.h"

class Camera{
public:
	Camera(void){
		yaw		= 0.0f;
		pitch	= 0.0f;
	};

	Camera(float pitch, float yaw, Vector3 position) {
		this->pitch		= pitch;
		this->yaw		= yaw;
		this->position	= position;
	}

	~Camera(void){
	};

	void UpdateCamera(float msec = 10.0f);

	//Builds a view matrix for the current camera variables, suitable for sending straight
	//to a vertex shader (i.e it's already an 'inverse camera matrix').
	Matrix4 BuildViewMatrix();

	//Gets position in world space
	Vector3 GetPosition() const { 
		return position;
	}

	//Sets position in world space
	void SetPosition(Vector3 val)
	{
		position = val;

	}

	//Gets yaw, in degrees
	float	GetYaw()   const { return yaw;}
	//Sets yaw, in degrees
	void	SetYaw(float y) {yaw = y;}

	//Gets pitch, in degrees
	float	GetPitch() const { return pitch;}
	//Sets pitch, in degrees
	void	SetPitch(float p) {pitch = p;}

protected:
	Vector3 position;
	float	yaw;
	float	pitch;
};