#pragma once

#include "CameraControllerType.h"

class Model;
class ModelMesh;

const int NUM_DIRECTIONS = 4;

class MechCameraController : public CameraControllerType
{
public:
	MechCameraController(Model* robot, vector<Model*> cockpitGlass, Camera* camera, Window* window);
	~MechCameraController() {}

	void ApplyInputs(const float& msec) override;
	void ApplyCustomRotation(const float& pitch, const float& yaw, 
		const float& msec) override;


private:
	void ApplyMovement(const float& msec) override;
	void ApplyRotation(const float& msec) override;

	void Move(Vector3 direction, float speed, int directionIndex);
	void Rotate(Vector3 axis, float speed, int directionIndex);

	void ShakeCamera(float msec);
	void GenerateCameraShakePositions();

	float RandomFloat(const float min, const float max);

	Model* robot;
	vector<Model*> cockpitGlass;
	ModelMesh* turrets;
	float previousYaw;
	float previousPitch;
	float frameCount;
	float rotationCount;

	bool shakeCamera;
	bool movementTriggers[NUM_DIRECTIONS];

	Vector3 directions[NUM_DIRECTIONS];
	vector<Vector3> screenShakePositions;
	Vector3 originalPosition;
	int currentShakePosition = 0;
};

