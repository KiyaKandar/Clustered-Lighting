#include "CameraController.h"

#define MOVEMENT_DAMPING 0.5f

CameraController::CameraController(Camera* camera, Window* window)
{
	this->camera = camera;
	this->window = window;
}

void CameraController::ApplyInputs(float msec)
{
	ApplyRotation(msec);
	ApplyMovement(msec);

	//Set camera pitch and yaw
	camera->SetPitch(pitch);
	camera->SetYaw(yaw);
}

void CameraController::ApplyMovement(float msec)
{
	float speed = msec * MOVEMENT_DAMPING;

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
		camera->SetPosition(camera->GetPosition() + Vector3(0, 1, 0) * speed);
	}

	if (window->GetKeyboard()->KeyDown(KEYBOARD_C)) {
		camera->SetPosition(camera->GetPosition() + Vector3(0, -1, 0) * speed);
	}
}

void CameraController::ApplyRotation(float msec)
{
	pitch	-= (Window::GetMouse()->GetRelativePosition().y);
	yaw		-= (Window::GetMouse()->GetRelativePosition().x);
}

