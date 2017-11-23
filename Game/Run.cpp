#pragma comment(lib, "nclgl.lib")
#pragma comment(lib, "Profiler.lib")
#pragma comment(lib, "assimp-vc140-mt.lib")

#include "CameraControl/SimpleCameraController.h"
#include "../Profiler/Profiler.h"

#include "../NCLGL/Rendering/View/window.h"
#include "../NCLGL/Rendering/Renderer/Renderer.h"
#include "../NCLGL/ASSIMP/Model.h"
#include "../NCLGL/GraphicsSettings/SettingsType/GConfiguration.h"
#include "GraphicsConfiguration/GLConfig.h"
#include "CameraControl/MechCameraController.h"

#include "Scenes/RobotScene.h"
#include "Scenes/HorrorScene.h"
#include "Scenes/SponzaScene.h"

/*
	CREDITS:
*/

const bool FULLSCREEN = true;

int main()
{
	Window* window = new Window("CFL", GLConfig::RESOLUTION.x, GLConfig::RESOLUTION.y, FULLSCREEN);
	window->LockMouseToWindow(true);
	window->ShowOSPointer(false);

	Camera* camera = new Camera(0, 0, Vector3(-875, 900, 0));
	Renderer* renderer = new Renderer(*window, camera);

	if (!renderer->HasInitialised() || !window->HasInitialised())
	{
		return -1;
	}

	Profiler* profiler = new Profiler(renderer, window, 1);
	profiler->AddSubSystemTimer("Renderer", &renderer->updateTimer);

	GConfiguration config(window, renderer, camera, GLConfig::RESOLUTION, profiler);
	config.InitialiseSettings();

	SimpleCameraController* camControl = new SimpleCameraController(camera, window);
	camControl->ApplyCustomRotation(-10, 270, 0);
	RobotScene::CreateShowroomScene(renderer, window, &camControl, camera);
	HorrorScene::CreateScaryScene(renderer);
	SponzaScene::CreateSponzaScene(renderer, camera, window);

	config.LinkToRenderer();
	window->UpdateWindow();

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
