#include "MechCameraController.h"

#include "../../nclgl/ASSIMP/Model.h"
#include "../../nclgl/ASSIMP/ModelMesh.h"
#include <random>
#include <chrono>

#define MOVEMENT_DAMPING 0.1f

const int NUM_SHAKE_POSITIONS = 1;

enum MOVEMENT_TRIGGERS
{
	FORWARD,
	BACK,
	LEFT,
	RIGHT
};

MechCameraController::MechCameraController(Model* robot, vector<Model*> cockpitGlass, Camera* camera, Window* window)
	: CameraControllerType(camera, window)
{
	this->camera = camera;
	this->window = window;
	this->robot = robot;
	this->cockpitGlass = cockpitGlass;
	turrets = robot->meshesByName["pCylinder45"];

	pitch = 0.0f;
	yaw = 0.0f;
	previousYaw = 0.0f;
	previousPitch = 0.0f;
	frameCount = 0.1f;
	rotationCount = 0.1f;

	shakeCamera = false;
	movementTriggers[FORWARD] = false;
	movementTriggers[BACK] = false;
	movementTriggers[LEFT] = false;
	movementTriggers[RIGHT] = false;

	directions[FORWARD] = Vector3(0, 0, -1);
	directions[BACK] = Vector3(0, 0, 1);
	directions[LEFT] = Vector3(0, 1, 0);
	directions[RIGHT] = Vector3(0, -1, 0);
}

void MechCameraController::ApplyInputs(const float& msec)
{
	ApplyRotation(msec);
	ApplyMovement(msec);

	//Set camera pitch and yaw
	camera->SetPitch(pitch);
	camera->SetYaw(yaw);
}

void MechCameraController::ApplyCustomRotation(const float& pitch, const float& yaw,
	const float& msec)
{
	this->pitch = pitch;
	this->yaw = yaw;
	previousYaw = yaw;

	ApplyRotation(msec);
}

void MechCameraController::ApplyMovement(const float& msec)
{
	const float speed = msec * MOVEMENT_DAMPING;

	if (window->GetKeyboard()->KeyDown(KEYBOARD_W)) 
	{
		movementTriggers[FORWARD] = true;
	}
	else if (window->GetKeyboard()->KeyDown(KEYBOARD_S))
	{
		movementTriggers[BACK] = true;
	}
	else if (window->GetKeyboard()->KeyDown(KEYBOARD_A)) 
	{
		movementTriggers[LEFT] = true;
	}
	else if (window->GetKeyboard()->KeyDown(KEYBOARD_D)) 
	{
		movementTriggers[RIGHT] = true;
	}

	if(shakeCamera)
	{
		ShakeCamera(msec);
	}
	else
	{
		for (int i = 0; i < NUM_DIRECTIONS; ++i)
		{
			if (movementTriggers[i])
			{
				if (i > 1) 
				{
					Rotate(directions[i], speed, i);
				}
				else
				{
					Move(directions[i], speed, i);
				}
				
			}
		}
	}
}

void MechCameraController::ApplyRotation(const float& msec)
{
	pitch -= (Window::GetMouse()->GetRelativePosition().y);
	yaw -= (Window::GetMouse()->GetRelativePosition().x);

	turrets->Rotate(Vector3(0, 1, 0), (camera->GetYaw() - previousYaw)/* / 100*/, 0);
	previousYaw = camera->GetYaw();

	//turrets->Rotate(Vector3(0, 0, -1), -(camera->GetPitch() - previousPitch), 0);
	//previousPitch = camera->GetPitch();
}

void MechCameraController::Move(Vector3 direction, float speed, int directionIndex)
{
	camera->SetPosition(camera->GetPosition() +
		Matrix4::Rotation(yaw, Vector3(0, 1, 0)) *  direction * speed);

	Vector3 robotsNewPosition = camera->GetPosition() + Vector3(-200, -900, 0);
	const float multiplier = (std::sinf(frameCount) + 1);
	robotsNewPosition = robotsNewPosition + ((Vector3(0, 20, 0) * multiplier));

	robot->Translate(robotsNewPosition);

	for each (Model* glass in cockpitGlass)
	{
		const Vector3 newGlassPosition = robotsNewPosition + Vector3(-100, 0, 0);
		glass->Translate(newGlassPosition);
	}

	frameCount += 0.1;

	if (multiplier < 0.001)
	{
		originalPosition = camera->GetPosition();
		shakeCamera = true;
		GenerateCameraShakePositions();
		movementTriggers[directionIndex] = false;
	}
}

void MechCameraController::Rotate(Vector3 axis, float speed, int directionIndex)
{
	//pitch -= 20.0f;// (Window::GetMouse()->GetRelativePosition().y);
	yaw -= 0.1f;// (Window::GetMouse()->GetRelativePosition().x);
	camera->SetYaw(yaw);
	previousYaw = camera->GetYaw();

	//turrets->Rotate(Vector3(0, 1, 0), (camera->GetYaw() - previousYaw)/* / 100*/, 0);
	//previousYaw = camera->GetYaw();

	const float multiplier = (std::sinf(rotationCount) + 1);

	robot->Rotate(axis, 0.1f);
	//robot->Translate(camera->GetPosition() + ((Vector3(0, 20, 0) * multiplier)));
	rotationCount += 0.1;

	if (multiplier < 0.001)
	{
		originalPosition = camera->GetPosition();
		shakeCamera = true;
		GenerateCameraShakePositions();
		movementTriggers[directionIndex] = false;
	}
}

void MechCameraController::ShakeCamera(float msec)
{
	if (currentShakePosition == screenShakePositions.size())
	{
		camera->SetPosition(originalPosition);
		shakeCamera = false;
	}
	else
	{
		//return a + f * (b - a);
		const Vector3 currentPosition = camera->GetPosition();
		const Vector3 nextPosition = (screenShakePositions[currentShakePosition] - currentPosition) * (msec / 25);
		const Vector3 newPosition = currentPosition + nextPosition;

		camera->SetPosition(newPosition);

		const Vector3 distanceToNextPos = newPosition - screenShakePositions[currentShakePosition];
		const float absX = abs(distanceToNextPos.x);
		const float absY = abs(distanceToNextPos.y);
		const float absZ = abs(distanceToNextPos.z);
		const Vector3 distance(absX, absY, absZ);

		if(distance.Length() < 0.1)
		{
			++currentShakePosition;
		}
	}
}

void MechCameraController::GenerateCameraShakePositions()
{
	for (int i = 0; i < NUM_SHAKE_POSITIONS; ++i)
	{
		const float x = camera->GetPosition().x + RandomFloat(-8, 8);
		const float y = camera->GetPosition().y + RandomFloat(-8, 8);
		const float z = camera->GetPosition().z + RandomFloat(-8, 8);

		screenShakePositions.push_back(Vector3(x, y, z));
	}
}

float MechCameraController::RandomFloat(const float min, const float max)
{
	return min + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (max - min)));
}
