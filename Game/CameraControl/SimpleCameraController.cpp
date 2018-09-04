#include "SimpleCameraController.h"

#define MOVEMENT_DAMPING 1.2f

SimpleCameraController::SimpleCameraController(Camera* camera, Window* window) 
	: CameraControllerType(camera, window)
{
	this->camera = camera;
	this->window = window;

	pitch = 0.0f;
	yaw = 0.0f;
}

void SimpleCameraController::ApplyInputs(const float& msec)
{
	ApplyRotation(msec);
	ApplyMovement(msec);

	//Set camera pitch and yaw
	camera->SetPitch(pitch);
	camera->SetYaw(yaw);
}

void SimpleCameraController::ApplyCustomRotation(const float& pitch, const float& yaw,
	const float& msec)
{
	this->pitch = pitch;
	this->yaw = yaw;

	ApplyRotation(msec);
}

void SimpleCameraController::Translate(Vector3 translation)
{
	camera->SetPosition(camera->GetPosition() + translation);
}

void SimpleCameraController::ApplyMovement(const float& msec)
{
	const float speed = msec * MOVEMENT_DAMPING;

	if (window->GetKeyboard()->KeyDown(KEYBOARD_W)) {
		camera->SetPosition(camera->GetPosition() + 
			Matrix4::Rotation(yaw, Vector3(0, 1, 0)) * Vector3(0, 0, -1) * speed);
	}

	if (window->GetKeyboard()->KeyDown(KEYBOARD_S)) {
		camera->SetPosition(camera->GetPosition() +
			Matrix4::Rotation(yaw, Vector3(0, 1, 0)) * Vector3(0, 0, 1) * speed);
	}

	if (window->GetKeyboard()->KeyDown(KEYBOARD_A)) {
		camera->SetPosition(camera->GetPosition() +
			Matrix4::Rotation(yaw, Vector3(0, 1, 0)) *  Vector3(-1, 0, 0) * speed);
	}

	if (window->GetKeyboard()->KeyDown(KEYBOARD_D)) {
		camera->SetPosition(camera->GetPosition() +
			Matrix4::Rotation(yaw, Vector3(0, 1, 0)) *  Vector3(1, 0, 0) * speed);
	}

	if (window->GetKeyboard()->KeyDown(KEYBOARD_SPACE)) {
		camera->SetPosition(camera->GetPosition() + Vector3(0, 0.4, 0) * speed);
	}

	if (window->GetKeyboard()->KeyDown(KEYBOARD_C)) {
		camera->SetPosition(camera->GetPosition() + Vector3(0, -0.4, 0) * speed);
	}
}

void SimpleCameraController::ApplyRotation(const float& msec)
{
	pitch	-= (Window::GetMouse()->GetRelativePosition().y);
	yaw		-= (Window::GetMouse()->GetRelativePosition().x);
}

