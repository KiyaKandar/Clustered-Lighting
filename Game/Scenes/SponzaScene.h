#pragma once

#include "../nclgl/Rendering/Renderer/Renderer.h"
#include "../CameraControl/CameraControllerType.h"
#include "../CameraControl/SimpleCameraController.h"

class SponzaScene
{
public:

	static void CreateSponzaScene(Renderer* renderer, Window* window, Camera* camera, CameraControllerType** camControl)
	{
		vector<pair<string, int>> files;
		files.push_back(make_pair("../sponza/sponza.obj", 1));

		vector<string> skybox =
		{
			"../Skyboxes/Nice/right.jpg",
			"../Skyboxes/Nice/left.jpg",
			"../Skyboxes/Nice/top.jpg",
			"../Skyboxes/Nice/bottom.jpg",
			"../Skyboxes/Nice/back.jpg",
			"../Skyboxes/Nice/front.jpg",
		};

		vector<string> reflections =
		{
			"../sponza/textures/spnza_bricks_a_diff.tga",
			"../sponza/textures/spnza_bricks_a_diff.tga",
			"../sponza/textures/spnza_bricks_a_diff.tga",
			"../sponza/textures/spnza_bricks_a_diff.tga",
			"../sponza/textures/spnza_bricks_a_diff.tga",
			"../sponza/textures/spnza_bricks_a_diff.tga",
		};

		Scene* scene = new Scene(skybox, reflections, files, Vector3(100, 1, 1));
		scene->InitialiseShadows(1, renderer);
		scene->LoadModels();
		scene->AddLight(new Light(Vector3(0, 1800, 200), Vector4(0.9, 0.7, 0.4, 1), 30000.0f, 1.5f), 0);
		scene->AddLight(new Light(Vector3(-630, 140, -200), Vector4(1.0f, (140.0f / 255.0f), 0.0f, 1), 150.0f, 1.0f), 1);
		scene->AddLight(new Light(Vector3(500, 140, -200), Vector4(1.0f, (140.0f / 255.0f), 0.0f, 1), 150.0f, 1.0f), 2);
		scene->AddLight(new Light(Vector3(-630, 140, 150), Vector4(1.0f, (140.0f / 255.0f), 0.0f, 1), 150.0f, 1.0f), 3);
		scene->AddLight(new Light(Vector3(500, 140, 150), Vector4(1.0f, (140.0f / 255.0f), 0.0f, 1), 150.0f, 1.0f), 4);

		Model* sponza = scene->GetModel("../sponza/sponza.obj");
		sponza->meshesByName["floor"]->isReflective = 1.0;
		sponza->meshesByName["floor"]->reflectionStrength = 0.2f;

		*camControl = new SimpleCameraController(camera, window);
		(*camControl)->ApplyCustomRotation(-10, 270, 0);

		renderer->AddScene(scene);
	}
};

