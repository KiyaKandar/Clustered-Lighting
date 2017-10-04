#pragma once

#include "../nclgl/Rendering/View/Camera.h"
#include "../nclgl/Rendering/View/Window.h"

class CameraController
{
public:
	CameraController(Camera* camera, Window* window);
	~CameraController() {}

	void ApplyInputs(const float& msec);
	void ApplyCustomRotation(const float& pitch, const float& yaw, const float& msec);

private:
	void ApplyMovement(const float& msec) const;
	void ApplyRotation(const float& msec);

	Camera* camera;
	Window* window;

	float pitch;
	float yaw;
};

