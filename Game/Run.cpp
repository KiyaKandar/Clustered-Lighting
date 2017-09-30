#pragma comment(lib, "nclgl.lib")
#pragma comment(lib, "Threading.lib")
#pragma comment(lib, "Profiler.lib")
#pragma comment(lib, "assimp-vc140-mt.lib")

#include "CameraController.h"
#include "../Profiler/Profiler.h"

#include "../NCLGL/window.h"
#include "../NCLGL/Renderer.h"
#include "../NCLGL/Model.h"
#include "../NCLGL/Light.h"
#include "../NCLGL/GConfiguration.h"
#include "Util.h"
#include "GLConfig.h"

void ApplyLightInput(Light* light, const Window& window);

int main()
{
	Window* window = new Window("CFL", GLConfig::RESOLUTION.x, GLConfig::RESOLUTION.y, false);
	window->LockMouseToWindow(true);
	window->ShowOSPointer(false);

	Camera* camera = new Camera(0, 0, Vector3(900, 600, 100));
	Renderer* renderer = new Renderer(*window, camera);

	if (!renderer->HasInitialised() || !window->HasInitialised())
	{
		return -1;
	}

	Profiler* profiler = new Profiler(renderer, window, 1);
	profiler->AddSubSystemTimer("Renderer", &renderer->updateTimer);

	CameraController* camControl = new CameraController(camera, window);

	GConfiguration config(renderer, camera, GLConfig::RESOLUTION);
	config.InitialiseSettings();
	config.LinkToRenderer();

	Model sponza("../sponza/sponza.obj");
	renderer->AddModel(&sponza);

	//Game loop...
	while (window->UpdateWindow() && !window->GetKeyboard()->KeyTriggered(KEYBOARD_ESCAPE)) {
		const float deltatime = window->GetTimer()->GetTimedMS();

		camControl->ApplyInputs(deltatime);

		profiler->Update(deltatime);
		renderer->Update(deltatime);
	}

	delete window;
	delete renderer;
	delete profiler;
	delete camControl;

    return 0;
}

void ApplyLightInput(Light* light, const Window& window) {
	bool moved = false;

	if (window.GetKeyboard()->KeyDown(KEYBOARD_UP)) {
		light->SetPosition(light->GetPosition() + Vector3(0, 0, 3));
		moved = true;
	}

	if (window.GetKeyboard()->KeyDown(KEYBOARD_DOWN)) {
		light->SetPosition(light->GetPosition() + Vector3(0, 0, -3));
		moved = true;
	}

	if (window.GetKeyboard()->KeyDown(KEYBOARD_LEFT)) {
		light->SetPosition(light->GetPosition() + Vector3(-3, 0, 0));
		moved = true;
	}

	if (window.GetKeyboard()->KeyDown(KEYBOARD_RIGHT)) {
		light->SetPosition(light->GetPosition() + Vector3(3, 0, 0));
		moved = true;
	}

	if (window.GetKeyboard()->KeyDown(KEYBOARD_O)) {
		light->SetPosition(light->GetPosition() + Vector3(0, 3, 0));
		moved = true;
	}

	if (window.GetKeyboard()->KeyDown(KEYBOARD_L)) {
		light->SetPosition(light->GetPosition() + Vector3(0, -3, 0));
		moved = true;
	}

	if (moved) std::cout << light->GetPosition() << std::endl;
}