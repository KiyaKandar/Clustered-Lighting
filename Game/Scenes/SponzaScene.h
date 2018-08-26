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
	static float frameCounter;
	static Vector3 worldLightPosition;
	static vector<Vector2> radii;

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

		Scene* scene = new Scene(skybox, skybox, files, Vector3(10, 10, 10), 0.5f);
		scene->InitialiseShadows(1, renderer);
		scene->LoadModels();
		GenerateLights(scene);


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
				float cosX = cosf(frameCounter);
				float sinZ = sinf(frameCounter);

				for (int i = 0; i < GLConfig::NUM_LIGHTS; ++i)
				{
					float newXPos = radii[i].x * cosX;
					float newZPos = radii[i].y * sinZ;

					Vector3 currentPosition = scene->GetLightPosition(i);

					float xIncrement = newXPos - currentPosition.x;
					float zIncrement = newZPos - currentPosition.z;

					scene->SetLightPosition(i, currentPosition + Vector3(xIncrement, 0, zIncrement));
				}

				frameCounter += msec / 10000.0f;
			}

		});

		renderer->AddScene(scene);
	}

	static void GenerateLights(Scene* scene)
	{
		for (int i = 0; i < GLConfig::NUM_LIGHTS; ++i)
		{
			float x = GetRandomFloat(-2000, 2000);
			float y = GetRandomFloat(100, 1000);
			float z = GetRandomFloat(-750, 750);

			float r = GetRandomFloat(0, 1);
			float g = GetRandomFloat(0, 1);
			float b = GetRandomFloat(0, 1);

			float bulbRadius = GetRandomFloat(30.0f, 110.0f);
			float cutOff = GetRandomFloat(1.5f, 1.5f);
			float intensity = GetRandomFloat(1.1f, 2.3f);

			Vector3 pos(x, y, z);

			scene->AddLight(new Light(pos, Vector4(r, g, b, 1), bulbRadius, cutOff, intensity), i);

			radii.push_back(Vector2(pos.x, pos.z));
		}
	}

	static float GetRandomFloat(const float min, const float max)
	{
		std::random_device rd{};
		std::mt19937 engine{ rd() };
		std::uniform_real_distribution<float> dist{ min, max };

		return dist(engine);
	}
};

