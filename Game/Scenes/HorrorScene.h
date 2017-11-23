#pragma once

#include "../nclgl/Rendering/Renderer/Renderer.h"
#include "../CameraControl/CameraControllerType.h"
#include "../CameraControl/SimpleCameraController.h"

class HorrorScene
{
public:
	static float xDirection;
	static int counter;
	static int framesWithLightOff;

	static void CreateScaryScene(Renderer* renderer)
	{
		vector<pair<string, int>> files;
		files.push_back(make_pair("../Models/centeredcube.obj", 1));
		files.push_back(make_pair("../models/Hellknight/hellknight.md5mesh", 1));

		vector<string> faces =
		{
			"../Skyboxes/Black/black.jpg",
			"../Skyboxes/Black/black.jpg",
			"../Skyboxes/Black/black.jpg",
			"../Skyboxes/Black/black.jpg",
			"../Skyboxes/Black/black.jpg",
			"../Skyboxes/Black/black.jpg",
		};

		Scene* scene = new Scene(faces, faces, files, Vector3(2, 1, 1), 0.1f);
		scene->InitialiseShadows(2, renderer);

		scene->AddLight(new Light(Vector3(0, 700, -10),
			Vector4(1, 1, 1, 1), 2000.0f, 10.5f, Vector4(0, -1, 0, 15)), 0);

		scene->AddLight(new Light(Vector3(-300, 700, -10),
			Vector4(1, 1, 1, 1), 2000.0f, 2.5f, Vector4(0.5, -1, 0, 15)), 1);


		scene->LoadModels();
		scene->particles = GenerateSmoke(150);

		Model* cube = scene->GetModel("../Models/centeredcube.obj");
		Model* hellknight = scene->GetModel("../models/Hellknight/hellknight.md5mesh");

		cube->Scale(Vector3(1000, 100, 1000));
		cube->Translate(Vector3(0, -100, 0));
		cube->SetbackupColourAttributeForAllSubMeshes(Vector4(0.1f, 0.1f, 0.1f, 1.0f));

		hellknight->Scale(Vector3(100, 100, 100));
		hellknight->Scale(Vector3(3.0f, 3.0f, 3.0f));
		hellknight->Translate(Vector3(0, 0, 0));
		hellknight->Rotate(Vector3(1, 0, 0), 90);
		hellknight->Rotate(Vector3(0, 1, 0), 180);

		scene->AddUpdateProcess([scene = scene](float msec)
		{
			if (counter <= 0)
			{
				std::random_device rd;
				std::mt19937 rng(rd()); //(Mersenne-Twister)
				std::uniform_int_distribution<int> uni(0, 150);

				*scene->lightWorkGroups = Vector3(1, 0, 0);
				counter = uni(rng);
			}
			else
			{
				*scene->lightWorkGroups = Vector3(2, 1, 1);
				counter = counter - msec;
			}

		});

		scene->AddUpdateProcess([scene = scene](float msec)
		{
			float direction = std::cosf(xDirection) * 0.45;
			xDirection += 0.05f * (msec / 10);
			scene->SetLightDirection(0, Vector4(direction, -1.0f, 0.0f, 20.0f));
		});

		renderer->AddScene(scene);
	}

	static vector<Particle> GenerateSmoke(int numParticles)
	{
		vector<Particle> particles;

		for (int i = 0; i < numParticles; ++i)
		{
			const Vector3 start(RandomFloat(-450, 450), RandomFloat(-500, -300), RandomFloat(-300, 300));
			const Vector3 translation(RandomFloat(-0.05, 0.1), RandomFloat(-0.05, 0.1), RandomFloat(-0.05, 0.1));
			const float size = RandomFloat(100, 300);
			const float decayRate = RandomFloat(0.005, 0.0015);
			const float whiteness = RandomFloat(0.1, 0.3);

			const Particle particle(start, translation, Vector4(whiteness, whiteness, whiteness, 1.0f), size, decayRate);
			particles.push_back(particle);
		}

		return particles;
	}

	static float RandomFloat(const float min, const float max)
	{
		return min + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (max - min)));
	}
};