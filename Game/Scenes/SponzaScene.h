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
	static vector<Vector2> radii;
	static Vector3 workGroups;

	static void CreateSponzaScene(Renderer* renderer, Camera* camera, Window* window)
	{
#ifdef DEMO_PRETTY
		CreatePrettyScene(renderer, camera, window);
#else
		CreateCLDemoScene(renderer, camera, window);
#endif
	}

private:
	static void CreatePrettyScene(Renderer* renderer, Camera* camera, Window* window);

	static void CreateCLDemoScene(Renderer* renderer, Camera* camera, Window* window);

	static void GenerateLights(Scene* scene);
	static float GetRandomFloat(const float min, const float max);
};

