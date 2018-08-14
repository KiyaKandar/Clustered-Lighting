#pragma once

#include "../nclgl/Rendering/Renderer/Renderer.h"
#include "../CameraControl/CameraControllerType.h"
#include "../CameraControl/SimpleCameraController.h"

class RobotScene
{
public:

	static float counter;
	static float waveCounter;
	static bool manual;
	static float cameraRotationDegrees;

	static void CreateShowroomScene(Renderer* renderer, Window* window, SimpleCameraController** camControl, Camera* camera)
	{
		vector<pair<string, int>> files;
		files.push_back(make_pair("../Models/glass.obj", 3));
		files.push_back(make_pair("../Models/Robot1.obj", 1));
		//files.push_back(make_pair("../tank/abrams/tank.3ds", 1));
		files.push_back(make_pair("../Models/centeredcube.obj", 1));
		files.push_back(make_pair("../Models/environmentcube.obj", 529));

		vector<string> faces =
		{
			"../Skyboxes/Nice/right.jpg",
			"../Skyboxes/Nice/left.jpg",
			"../Skyboxes/Nice/top.jpg",
			"../Skyboxes/Nice/bottom.jpg",
			"../Skyboxes/Nice/back.jpg",
			"../Skyboxes/Nice/front.jpg",
		};

		Scene* scene = new Scene(faces, faces, files, Vector3(1, 1, 1), 0.6f);
		scene->InitialiseShadows(1, renderer);
		scene->AddLight(new Light(Vector3(0, 1500, -2500), Vector4(0.9, 0.7, 0.4, 1), 70000000.0f, 7.9f), 0);

		scene->LoadModels();

		Model* glass = scene->GetModel("../Models/glass.obj");
		Model* enemy = scene->GetModel("../Models/Robot1.obj");
		Model* cube = scene->GetModel("../Models/centeredcube.obj");
		Model* environmentCube = scene->GetModel("../Models/environmentcube.obj");
		//Model* tank = scene->GetModel("../tank/abrams/tank.3ds");

		environmentCube->SetMetallicAttributeForAllSubMeshes(0.2f);
		environmentCube->SetRoughnessAttributeForAllSubMeshes(0.0f);

		cube->Scale(Vector3(100, 1000, 100));
		cube->Translate(Vector3(2500, 0, 0));
		cube->SetMetallicAttributeForAllSubMeshes(0.1f);
		cube->SetRoughnessAttributeForAllSubMeshes(0.5f);

		glass->Scale(Vector3(100, 1000, 100));
		glass->Translate(Vector3(4400, 0, 1000));

		glass->Scale(Vector3(100, 1000, 100), 1);
		glass->Translate(Vector3(730, 0, 1250), 1);

		glass->Scale(Vector3(100, 1000, 100), 2);
		glass->Translate(Vector3(730, 0, 750), 2);

		glass->SetbackupColourAttributeForAllSubMeshes(Vector4(0.0f, 0.0f, 0.8f, 0.3f));

		//tank->Rotate(Vector3(1, 0, 0), -90);
		//tank->Rotate(Vector3(0, 0, 1), 200);
		//tank->Translate(Vector3(4400, 1000, 1000));
		//tank->SetMetallicAttributeForAllSubMeshes(0.6f);
		//tank->SetRoughnessAttributeForAllSubMeshes(0.6f);

		enemy->Scale(Vector3(80, 80, 80));
		enemy->Translate(Vector3(400, 800, 1000));
		//enemy->SetReflectionAttributesForAllSubMeshes(1, 2.0f);
		enemy->SetbackupColourAttributeForAllSubMeshes(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
		enemy->meshesByName["pCylinder45"]->baseColour = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
		enemy->SetMetallicAttributeForAllSubMeshes(1.0f);
		enemy->SetRoughnessAttributeForAllSubMeshes(0.0f);

		GenerateEnvironment(environmentCube);

		scene->AddUpdateProcess([cube = environmentCube](float msec)
		{
			float b = sinf(waveCounter * 10) / 2;
			cube->SetbackupColourAttributeForAllSubMeshes(Vector4(1.0f, b, 0, 1.0f));

			for (int i = 0; i < 265; ++i)
			{
				cube->Translate(cube->GetPosition(i) + Vector3(0, 10 * cosf((i + 265)* waveCounter), 0), i);
			}

			for (int i = 265; i < 529; ++i)
			{
				cube->Translate(cube->GetPosition(i) + Vector3(0, 10 * cosf((i + 265)* waveCounter), 0), i);
			}

			waveCounter += msec / 100000.0f;
		});

		scene->AddUpdateProcess([window = window, camControl = camControl, camera = camera](float msec)
		{
			if (window->GetKeyboard()->KeyTriggered(KEYBOARD_M))
			{
				manual = !manual;
			}

			if ((camera->GetPosition() - Vector3(2500, 1075, 0)).Length() > 1.0f && !manual)
			{
				const Vector3 currentPosition = camera->GetPosition();
				const Vector3 nextPosition = (Vector3(2500, 1075, 0) - currentPosition) * (msec / 550);
				const Vector3 newPosition = currentPosition + nextPosition;

				camera->SetPosition(newPosition);
			}

			if (!manual) 
			{
				cameraRotationDegrees = cameraRotationDegrees + (2 * (msec / 100));
				(*camControl)->ApplyCustomRotation(camera->GetPitch(), cameraRotationDegrees, msec);
			}
		});

		renderer->AddScene(scene);
	}

	static void GenerateEnvironment(Model* cube)
	{
		int xMultiplier = 0;
		int zMultiplier = 0;

		for (int i = 0; i < 529; ++i)
		{
			cube->Scale(Vector3(100, 600, 100), i);
		}

		for (int i = 0; i < 529; ++i)
		{
			cube->Translate(Vector3(xMultiplier * 2, 0, zMultiplier * 2) + Vector3(0, -300, -2300), i);

			xMultiplier += 100;

			if (xMultiplier > 2300)
			{
				xMultiplier = 0;
				zMultiplier += 100;
			}
		}
	}
};

