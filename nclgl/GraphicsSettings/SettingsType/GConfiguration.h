#pragma once

#include "../Settings/SSAO.h"
#include "../Settings/Shadows.h"
#include "../Settings/BPLighting.h"
#include "../Settings/GBuffer.h"
#include "../Settings/Bloom.h"
#include "../Settings/MotionBlur.h"
#include "../../Maths/Vector2.h"
#include "../Profiler/Profiler.h"
#include "../../Assets/Skybox.h"
#include "../../Assets/ParticleSystem.h"

#include <vector>
#include "../Settings/DepthPass.h"

class Renderer;
class Camera;

class GConfiguration
{
public:
	GConfiguration(Window* window, Renderer* renderer, Camera* camera, Vector2 resolution, Profiler* profiler);

	~GConfiguration();

	void InitialiseSettings();
	void LinkToRenderer();

private:
	Vector2		resolution;
	Renderer*	renderer;
	Camera*		camera;
	Profiler* profiler;
	Window* window;

	//Settings
	DepthPass* depthPass;
	GBuffer* SGBuffer;
	SSAO* ssao;
	BPLighting* lighting;
	Bloom* bloom;
	AmbientTextures* ambTex;
	MotionBlur* motionBlur;
	Skybox* skybox;
	ParticleSystem* particles;
};

