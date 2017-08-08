#pragma once

#include "../nclgl/Camera.h"
#include "../nclgl/Window.h"

class CameraController
{
public:
	CameraController(Camera* camera, Window* window);
	~CameraController() {}

	void ApplyInputs(float msec);

private:
	void ApplyMovement(float msec);
	void ApplyRotation(float msec);

	Camera* camera;
	Window* window;

	float pitch;
	float yaw;
};

