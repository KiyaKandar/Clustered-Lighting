#pragma once

#include "CameraControllerType.h"

class SimpleCameraController : public CameraControllerType
{
public:
	SimpleCameraController(Camera* camera, Window* window);
	~SimpleCameraController() {}

	void ApplyInputs(const float& msec) override;

	void ApplyCustomRotation(const float& pitch, const float& yaw, const float& msec) override;
	void Translate(Vector3 translation);

private:
	void ApplyMovement(const float& msec) override;
	void ApplyRotation(const float& msec) override;
};

