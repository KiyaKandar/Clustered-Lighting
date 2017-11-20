#pragma once

#include "../nclgl/Rendering/Renderer/Renderer.h"
#include "../CameraControl/CameraControllerType.h"
#include "../CameraControl/SimpleCameraController.h"

class HorrorScene
{
public:
	static void CreateScaryScene(Renderer* renderer, Window* window, Camera* camera, CameraControllerType** camControl)
	{
		vector<pair<string, int>> files;
		files.push_back(make_pair("../centeredcube.obj", 1));
		files.push_back(make_pair("../models/Hellknight/hellknight.md5mesh", 1));

		vector<string> faces;

		Scene* scene = new Scene(faces, faces, files, Vector3(1, 1, 1));
		scene->InitialiseShadows(1, renderer);
		scene->AddLight(new Light(Vector3(0, 700, -10),
			Vector4(1, 1, 1, 1), 2000.0f, 10.5f, Vector4(0, -1, 0, 15)), 0);
		scene->LoadModels();
		scene->particles = GenerateSmoke();

		Model* cube = scene->GetModel("../centeredcube.obj");
		Model* hellknight = scene->GetModel("../models/Hellknight/hellknight.md5mesh");

		cube->Scale(Vector3(1000, 100, 1000));
		cube->Translate(Vector3(0, -100, 0));
		cube->SetbackupColourAttributeForAllSubMeshes(Vector4(0.1f, 0.1f, 0.1f, 1.0f));

		hellknight->Scale(Vector3(100, 100, 100));
		hellknight->Scale(Vector3(3.0f, 3.0f, 3.0f));
		hellknight->Translate(Vector3(0, 0, 0));
		hellknight->Rotate(Vector3(1, 0, 0), 90);
		hellknight->Rotate(Vector3(0, 1, 0), 180);

		int* counter = new int(0.0f);
		//scene->

		scene->AddUpdateProcess([scene = scene, counter = counter](float msec)
		{
			if ((*counter) % 20 == 0)
			{
				*scene->lightWorkGroups = Vector3(0, 0, 0);
			}
			else
			{
				*scene->lightWorkGroups = Vector3(1, 1, 1);
			}

			*counter = *counter + 1;
		});

		*camControl = new SimpleCameraController(camera, window);
		(*camControl)->ApplyCustomRotation(-10, 270, 0);

		renderer->AddScene(scene);
	}

	static vector<Particle> GenerateSmoke()
	{
		Particle a(Vector3(0, -400, 0), Vector3(0.1, 0, 0), 100, 0.0005);
		Particle b(Vector3(120, -400, 0), Vector3(0.1, 0, 0), 100, 0.0005);

		vector<Particle> particles = { a, b };
		return particles;
	}
};

