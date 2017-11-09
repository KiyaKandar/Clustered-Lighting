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

void CreateSponzaScene(Renderer* renderer);
void CreateShowroomScene(Renderer* renderer);

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

	//CreateSponzaScene(renderer);
	CreateShowroomScene(renderer);

	GConfiguration config(window, renderer, camera, GLConfig::RESOLUTION, profiler);
	config.InitialiseSettings();
	config.LinkToRenderer();

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

void CreateSponzaScene(Renderer* renderer)
{
	Model* sponza = new Model("../sponza/sponza.obj");
	renderer->AddModel(sponza);
}

void CreateShowroomScene(Renderer* renderer)
{
	Model* tank = new Model("../tank/T-90/T-90.obj");
	tank->Scale(Vector3(100, 100, 100));
	renderer->AddModel(tank);

	Model* tank1 = new Model("../tank/T-90/T-90.obj");
	tank1->Scale(Vector3(100, 100, 100));
	tank1->Translate(Vector3(450, 0, 0));
	renderer->AddModel(tank1);

	Model* tank2 = new Model("../tank/T-90/T-90.obj");
	tank2->Scale(Vector3(100, 100, 100));
	tank2->Translate(Vector3(-450, 0, 0));
	renderer->AddModel(tank2);

	Model* cube = new Model("../centeredcube.obj");
	cube->Scale(Vector3(1000, 100, 1000));
	cube->Translate(Vector3(0, -100, 0));
	renderer->AddModel(cube);

	Model* hellknight = new Model("../models/Hellknight/hellknight.md5mesh");
	hellknight->Scale(Vector3(3.0f, 3.0f, 3.0f));
	hellknight->Translate(Vector3(750, 0, 0));
	hellknight->Rotate(Vector3(1, 0, 0), 90);
	hellknight->Rotate(Vector3(0, 1, 0), 180);
	renderer->AddModel(hellknight);
}