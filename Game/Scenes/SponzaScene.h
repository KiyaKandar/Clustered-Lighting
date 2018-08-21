#pragma once

#include "../nclgl/Rendering/Renderer/Renderer.h"
#include "../CameraControl/CameraControllerType.h"
#include "../CameraControl/SimpleCameraController.h"

class SponzaScene
{
public:
	static int currentPositionIndex;
	static bool manual;
	static bool moveLight;
	static Vector3 worldLightPosition;

	static void CreateSponzaScene(Renderer* renderer, Camera* camera, Window* window)
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

		Scene* scene = new Scene(skybox, skybox, files, Vector3(100, 1, 1), 0.5f);
		scene->InitialiseShadows(1, renderer);
		scene->LoadModels();

		scene->AddLight(new Light(worldLightPosition, Vector4(0.9, 0.7, 0.4, 1), 10000.0f, 4.5f), 0);
		scene->AddLight(new Light(Vector3(-630, 140, -200), Vector4(1.0f, (140.0f / 255.0f), 0.0f, 1), 150.0f, 1.0f), 1);
		scene->AddLight(new Light(Vector3(500, 140, -200), Vector4(1.0f, (140.0f / 255.0f), 0.0f, 1), 150.0f, 1.0f), 2);
		scene->AddLight(new Light(Vector3(-630, 140, 150), Vector4(1.0f, (140.0f / 255.0f), 0.0f, 1), 150.0f, 1.0f), 3);
		scene->AddLight(new Light(Vector3(500, 140, 150), Vector4(1.0f, (140.0f / 255.0f), 0.0f, 1), 150.0f, 1.0f), 4);


		scene->AddUpdateProcess([camera = camera](float msec) 
		{
		});

		vector<Vector3> positions =
		{
			Vector3(-1300, 640, -550),
			Vector3(1300, 640, -550),
			Vector3(1300, 640, 550),
			Vector3(-1300, 640, 550)
		};

		scene->AddUpdateProcess([window = window](float msec)
		{
			if (window->GetKeyboard()->KeyTriggered(KEYBOARD_M))
			{
				manual = !manual;
			}
		});

		scene->AddUpdateProcess([window = window](float msec)
		{
			if (window->GetKeyboard()->KeyTriggered(KEYBOARD_J))
			{
				moveLight = !moveLight;
			}
		});

		scene->AddUpdateProcess([scene = scene, positions = positions, camera = camera](float msec)
		{
			if (!manual)
			{
				if (currentPositionIndex == positions.size())
				{
					currentPositionIndex = 0;
				}

				const Vector3 currentPosition = camera->GetPosition();
				const Vector3 nextPosition = (positions[currentPositionIndex] - currentPosition);
				const Vector3 newPosition = currentPosition + (nextPosition  * (msec / 9000));

				camera->SetPosition(newPosition);

				const Vector3 distanceToNextPos = newPosition - positions[currentPositionIndex];

				if (distanceToNextPos.Length() < 50.0f)
				{
					++currentPositionIndex;
				}
			}

			if (moveLight)
			{
				Vector3 currentPosition = scene->GetLightPosition(0);
				scene->SetLightPosition(0, currentPosition + Vector3(0, 0, 0.2));
			}

			//worldLightPosition = worldLightPosition + Vector3(0, 0, 0.1);
			//scene->SetLightPosition(0, worldLightPosition);
		});

		renderer->AddScene(scene);
	}
};

