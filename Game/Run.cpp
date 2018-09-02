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

void WriteConfigurationToShader();

int main()
{
	WriteConfigurationToShader();

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

void WriteConfigurationToShader()
{
	std::vector<string> lines;
	string filePath = "../Shaders/compute/configuration.glsl";
	ifstream file;
	string temp;

	file.open(filePath.c_str());
	if (!file.is_open()) {
		return;
	}

	while (!file.eof()) {
		getline(file, temp);
		lines.push_back(temp);
	}

	file.close();
	lines[1] = "const int numLights = " + std::to_string(GLConfig::NUM_LIGHTS) + ";";
	lines[2] = "const vec3 tilesOnAxes = vec3(" 
		+ std::to_string(GLConfig::NUM_X_AXIS_TILES) + ", "
		+ std::to_string(GLConfig::NUM_Y_AXIS_TILES) + ", "
		+ std::to_string(GLConfig::NUM_Z_AXIS_TILES)
		+ ");";

	std::ofstream ofs;
	ofs.open("../Shaders/compute/configuration.glsl", std::ofstream::out | std::ofstream::trunc);

	for (const string& line : lines)
	{
		ofs << line << std::endl;
	}
	ofs.close();
}