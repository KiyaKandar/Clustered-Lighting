#include "SponzaScene.h"

int SponzaScene::currentPositionIndex = 0;
bool SponzaScene::manual = true;
bool SponzaScene::moveLight = false;

vector<Vector2> SponzaScene::radii = vector<Vector2>();
float SponzaScene::frameCounter = 0.0f;

#if   defined DEMO_PRETTY
Vector3 SponzaScene::workGroups = Vector3(10, 10, 1);
#elif   defined DEMO_100_LIGHTS
Vector3 SponzaScene::workGroups = Vector3(10, 10, 1);
#elif defined DEMO_512_LIGHTS
Vector3 SponzaScene::workGroups = Vector3(8, 8, 8);
#elif defined DEMO_1024_LIGHTS
Vector3 SponzaScene::workGroups = Vector3(16, 8, 8);
#elif defined DEMO_2048_LIGHTS
Vector3 SponzaScene::workGroups = Vector3(16, 16, 8);
#endif


void SponzaScene::CreatePrettyScene(Renderer* renderer, Camera* camera, Window* window)
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

	Scene* scene = new Scene(skybox, skybox, files, workGroups, 0.5f);
	scene->LoadModels();

	scene->AddLight(new Light(Vector3(10, 1800, 200), Vector4(0.9, 0.7, 0.4, 1), 10000.0f, 1.0f, 4.5f), 0);
	scene->AddLight(new Light(Vector3(-630, 140, -200), Vector4(1.0f, (140.0f / 255.0f), 0.0f, 1), 150.0f, 1.5f, 1.0f), 1);
	scene->AddLight(new Light(Vector3(500, 140, -200), Vector4(1.0f, (140.0f / 255.0f), 0.0f, 1), 150.0f, 1.5f, 1.0f), 2);
	scene->AddLight(new Light(Vector3(-630, 140, 150), Vector4(1.0f, (140.0f / 255.0f), 0.0f, 1), 150.0f, 1.5f, 1.0f), 3);
	scene->AddLight(new Light(Vector3(500, 140, 150), Vector4(1.0f, (140.0f / 255.0f), 0.0f, 1), 150.0f, 1.5f, 1.0f), 4);

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

	scene->AddUpdateProcess([positions = positions, camera = camera](float msec)
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

	});

	renderer->AddScene(scene);
}

void SponzaScene::CreateCLDemoScene(Renderer* renderer, Camera* camera, Window* window)
{
	vector<pair<string, int>> files;
	files.push_back(make_pair("../sponza/sponza.obj", 1));
	files.push_back(make_pair("../Models/glass.obj", 1));

	vector<string> skybox =
	{
		"../Skyboxes/Nice/right.jpg",
		"../Skyboxes/Nice/left.jpg",
		"../Skyboxes/Nice/top.jpg",
		"../Skyboxes/Nice/bottom.jpg",
		"../Skyboxes/Nice/back.jpg",
		"../Skyboxes/Nice/front.jpg",
	};

	Scene* scene = new Scene(skybox, skybox, files, workGroups, 0.5f);
	scene->LoadModels();
	GenerateLights(scene);
	scene->ambient = 0.1f;

	Model* glass = scene->GetModel("../Models/glass.obj");
	glass->Scale(Vector3(105, 105, 105));
	glass->Translate(Vector3(-3500 + (500 * (13 - 5)), 500, -450));
	glass->SetbackupColourAttributeForAllSubMeshes(Vector4(0.0f, 0.0f, 0.8f, 0.5f));

	scene->AddUpdateProcess([scene = scene, window = window](float msec)
	{
		if (window->GetKeyboard()->KeyTriggered(KEYBOARD_J))
		{
			moveLight = !moveLight;
		}

		if (moveLight)
		{
			Vector3 currentPosition = scene->GetLightPosition(0);
			scene->SetLightPosition(0, currentPosition + Vector3(0, 0, 0.2));
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

void SponzaScene::GenerateLights(Scene* scene)
{
	for (int i = 0; i < GLConfig::NUM_LIGHTS; ++i)
	{
		float x = GetRandomFloat(-2000, 2000);
		float y = GetRandomFloat(100, 1000);
		float z = GetRandomFloat(-750, 750);

		float r = GetRandomFloat(0.1, 1);
		float g = GetRandomFloat(0.1, 1);
		float b = GetRandomFloat(0.1, 1);

		float bulbRadius = 100.0f;
		float cutOff = 1.5f;
		float intensity = GetRandomFloat(1.1f, 2.3f);

		Vector3 pos(x, y, z);

		scene->AddLight(new Light(pos, Vector4(r, g, b, 1), bulbRadius, cutOff, intensity), i);

		radii.push_back(Vector2(pos.x, pos.z));
	}
}

float SponzaScene::GetRandomFloat(const float min, const float max)
{
	std::random_device rd{};
	std::mt19937 engine{ rd() };
	std::uniform_real_distribution<float> dist{ min, max };
	return dist(engine);
}