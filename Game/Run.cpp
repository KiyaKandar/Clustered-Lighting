#pragma comment(lib, "nclgl.lib")
#pragma comment(lib, "Profiler.lib")
#pragma comment(lib, "assimp-vc140-mt.lib")

#include "CameraControl/CameraController.h"
#include "../Profiler/Profiler.h"

#include "../NCLGL/Rendering/View/window.h"
#include "../NCLGL/Rendering/Renderer/Renderer.h"
#include "../NCLGL/ASSIMP/Model.h"
#include "../NCLGL/Utility/Light.h"
#include "../NCLGL/GraphicsSettings/SettingsType/GConfiguration.h"
#include "GraphicsConfiguration/GLConfig.h"

const bool FULLSCREEN = false;

int main()
{
	Window* window = new Window("CFL", GLConfig::RESOLUTION.x, GLConfig::RESOLUTION.y, FULLSCREEN);
	window->LockMouseToWindow(true);
	window->ShowOSPointer(false);

	Camera* camera = new Camera(0, 0, Vector3(1200, 600, 150));
	Renderer* renderer = new Renderer(*window, camera);

	if (!renderer->HasInitialised() || !window->HasInitialised())
	{
		return -1;
	}

	Profiler* profiler = new Profiler(renderer, window, 1);
	profiler->AddSubSystemTimer("Renderer", &renderer->updateTimer);

	CameraController* camControl = new CameraController(camera, window);
	camControl->ApplyCustomRotation(-10, 75, 0);

	GConfiguration config(renderer, camera, GLConfig::RESOLUTION, profiler);
	config.InitialiseSettings();
	config.LinkToRenderer();

	Model sponza("../sponza/sponza.obj");
	renderer->AddModel(&sponza);

	//Model tank("../tank/Abrams_BF3.obj");
	//tank.Scale(Vector3(100, 100, 100));
	//renderer->AddModel(&tank);

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