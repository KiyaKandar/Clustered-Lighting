#pragma once

#include "../nclgl/Rendering/Renderer/Renderer.h"
#include "../CameraControl/CameraControllerType.h"
#include "../CameraControl/SimpleCameraController.h"

class RobotScene
{
public:
	static void CreateShowroomScene(Renderer* renderer, Window* window, Camera* camera, CameraControllerType** camControl)
	{
		vector<pair<string, int>> files;
		files.push_back(make_pair("../tank/abrams/tank.3ds", 2));
		files.push_back(make_pair("../centeredcube.obj", 9));
		files.push_back(make_pair("../glass.obj", 3));
		files.push_back(make_pair("../ModifiedRobot.obj", 1));
		files.push_back(make_pair("../Robot1.obj", 2));
		files.push_back(make_pair("../models/Hellknight/hellknight.md5mesh", 1));
		//files.push_back(make_pair("../models/cobblestones/Cobblestones3/Files/untitled.obj", 10));

		vector<string> faces =
		{
			"../Skyboxes/scifi/darkskies_rt.tga",
			"../Skyboxes/scifi/darkskies_lf.tga",
			"../Skyboxes/scifi/darkskies_up.tga",
			"../Skyboxes/scifi/darkskies_dn.tga",
			"../Skyboxes/scifi/darkskies_bk.tga",
			"../Skyboxes/scifi/darkskies_ft.tga",
		};

		//vector<string> faces =
		//{
		//	"../Skyboxes/Nice/right.jpg",
		//	"../Skyboxes/Nice/left.jpg",
		//	"../Skyboxes/Nice/top.jpg",
		//	"../Skyboxes/Nice/bottom.jpg",
		//	"../Skyboxes/Nice/back.jpg",
		//	"../Skyboxes/Nice/front.jpg",
		//};

		Scene* scene = new Scene(faces, faces, files, Vector3(100, 1, 1));
		scene->InitialiseShadows(2, renderer);
		//scene->AddLight(new Light(Vector3(0, 1800, -2000), Vector4(0.9, 0.7, 0.4, 1), 1000000.0f, 0.5f), 0);
		//scene->AddLight(new Light(Vector3(0, 1800, 2000), Vector4(0.9, 0.7, 0.4, 1), 1000000.0f, 0.5f), 1);

		scene->LoadModels();

		Model* cube = scene->GetModel("../centeredcube.obj");
		Model* glass = scene->GetModel("../glass.obj");
		Model* hellknight = scene->GetModel("../models/Hellknight/hellknight.md5mesh");
		Model* tank = scene->GetModel("../tank/abrams/tank.3ds");
		Model* robot = scene->GetModel("../ModifiedRobot.obj");
		Model* simpleRobot = scene->GetModel("../Robot1.obj");
		//Model* stones = scene->GetModel("../models/cobblestones/Cobblestones3/Files/untitled.obj");

		//for (int i = 0; i < 10; ++i)
		//{
		//	stones->Scale(Vector3(100, 100, 100), i);
		//	stones->Translate(Vector3(500, -55, (i * 750) - 2700), i);
		//}


		cube->Scale(Vector3(1000, 100, 1000));
		cube->Translate(Vector3(0, -100, 0));

		glass->Scale(Vector3(20, 100, 100));
		glass->Translate(Vector3(-700, 850, 0));

		glass->Scale(Vector3(75, 100, 20), 1);
		glass->Translate(Vector3(-800, 850, 100), 1);

		glass->Scale(Vector3(75, 100, 20), 2);
		glass->Translate(Vector3(-800, 850, -100), 2);

		cube->Scale(Vector3(1000, 100, 1000), 1);
		cube->Translate(Vector3(200, -100, 0), 1);
		cube->Scale(Vector3(1000, 100, 1000), 2);
		cube->Translate(Vector3(-2000, -100, 0), 2);
		cube->Scale(Vector3(1000, 100, 1000), 3);
		cube->Translate(Vector3(0, -100, -2000), 3);
		cube->Scale(Vector3(1000, 100, 1000), 4);
		cube->Translate(Vector3(200, -100, -2000), 4);
		cube->Scale(Vector3(1000, 100, 1000), 5);
		cube->Translate(Vector3(-2000, -100, -2000), 5);

		cube->Scale(Vector3(1000, 100, 1000), 6);
		cube->Translate(Vector3(0, -100, 2000), 6);
		cube->Scale(Vector3(1000, 100, 1000), 7);
		cube->Translate(Vector3(200, -100, 2000), 7);
		cube->Scale(Vector3(1000, 100, 1000), 8);
		cube->Translate(Vector3(-2000, -100, 2000), 8);

		//cube->SetReflectionAttributesForAllSubMeshes(1, 1.5f);
		cube->SetbackupColourAttributeForAllSubMeshes(Vector4((192.0f / 256.0f), (192.0f / 256.0f), (192.0f / 256.0f), 1.0f));

		glass->SetReflectionAttributesForAllSubMeshes(1, 1.0f);
		glass->SetbackupColourAttributeForAllSubMeshes(Vector4(0.0f, 0.0f, 0.0f, 0.4f));

		//hellknight->Scale(Vector3(100, 100, 100));
		hellknight->Scale(Vector3(3.0f, 3.0f, 3.0f));
		hellknight->Translate(Vector3(750, 0, 0));
		hellknight->Rotate(Vector3(1, 0, 0), 90);
		hellknight->Rotate(Vector3(0, 1, 0), 180);

		tank->Translate(Vector3(450, 0, 0), 1);
		tank->Rotate(Vector3(1, 0, 0), -90);
		tank->Rotate(Vector3(1, 0, 0), -90, 1);
		tank->Rotate(Vector3(0, 0, 1), 180);
		tank->Rotate(Vector3(0, 0, 1), 180, 1);

		robot->Scale(Vector3(80, 80, 80));
		robot->Translate(Vector3(-1000, -20, 0));
		robot->SetReflectionAttributesForAllSubMeshes(1, 2.0f);
		robot->SetbackupColourAttributeForAllSubMeshes(Vector4(0.0f, 0.0f, 0.0f, 1.0f));
		robot->meshesByName["pCylinder45"]->baseColour = Vector4(0.7f, 0.0f, 0.0f, 1.0f);

		simpleRobot->Scale(Vector3(80, 80, 80));
		simpleRobot->Translate(Vector3(-1000, -10, -1000));
		simpleRobot->Scale(Vector3(80, 80, 80), 1);
		simpleRobot->Translate(Vector3(-1000, -10, 1000), 1);
		simpleRobot->SetReflectionAttributesForAllSubMeshes(1, 2.0f);
		simpleRobot->SetbackupColourAttributeForAllSubMeshes(Vector4(0.0f, 0.0f, 0.0f, 1.0f));
		simpleRobot->meshesByName["pCylinder45"]->baseColour = Vector4(0.7f, 0.0f, 0.0f, 1.0f);


		vector<Model*> cockpit;
		cockpit.push_back(glass);

		//float* counter = new float(0.0f);
		//bool* triggered = new bool(false);

		//scene->AddUpdateProcess([triggered = triggered, glass = glass, counter = counter, window = window](float msec)
		//{
		//	if (window->GetKeyboard()->KeyTriggered(KEYBOARD_F))
		//	{
		//		*triggered = true;
		//	}

		//	if (*triggered) 
		//	{
		//		if (glass->meshes[0]->baseColour.w > 0.1f)
		//		{
		//			//glass->meshes[0]->baseColour = glass->meshes[0]->baseColour - Vector4(0.0f, 0.0f, 0.0f, *counter);
		//			glass->SetbackupColourAttributeForAllSubMeshes(glass->meshes[0]->baseColour - Vector4(0.0f, 0.0f, 0.0f, *counter));
		//		}

		//		*counter += 0.00009f / msec;
		//	}
		//});

		//*camControl = new MechCameraController(robot, cockpit, camera, window);
		//(*camControl)->ApplyCustomRotation(-10, 270, 0);

		Particle a(Vector3(0, -400, 0), Vector3(0.1, 0, 0), 100, 0.0005);
		Particle b(Vector3(120, -400, 0), Vector3(0.1, 0, 0), 100, 0.0005);

		vector<Particle> particles = { a, b };
		scene->particles = particles;

		*camControl = new SimpleCameraController(camera, window);
		(*camControl)->ApplyCustomRotation(-10, 270, 0);

		renderer->AddScene(scene);
	}
};

