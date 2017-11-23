#pragma once

#include "../nclgl/Rendering/View/Camera.h"
#include "../nclgl/Rendering/View/Window.h"

class CameraControllerType
{
public:
	CameraControllerType(Camera* camera, Window* window)
	{
		this->camera = camera;
		this->window = window;

		pitch = 0.0f;
		yaw = 0.0f;
	}

	virtual ~CameraControllerType()
	{
	}

	virtual void ApplyInputs(const float& msec) = 0;
	virtual void ApplyCustomRotation(const float& pitch, const float& yaw, 
		const float& msec) = 0;

	float pitch;
	float yaw;

protected:
	virtual void ApplyMovement(const float& msec) = 0;
	virtual void ApplyRotation(const float& msec) = 0;

	Camera* camera;
	Window* window;
};

