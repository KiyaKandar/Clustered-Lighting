#pragma once

#include "SSAO.h"
#include "Shadows.h"
#include "BPLighting.h"
#include "GBuffer.h"
#include "Bloom.h"
#include "Vector2.h"

#include <vector>

class Renderer;
class Camera;

class GConfiguration
{
public:
	GConfiguration(Renderer* renderer, Camera* camera, Vector2 resolution = Vector2(800, 600));

	~GConfiguration();

	std::vector<GSetting*>* GetAllSettings()
	{
		return &settings;
	}

	void InitialiseSettings();
	void LinkToRenderer();

private: 
	Vector2		resolution;
	Renderer*	renderer;
	Camera*		camera;

	std::vector<GSetting*> settings;

	//Settings
	Shadows* shadows;
	GBuffer* SGBuffer;
	SSAO* ssao;
	BPLighting* lighting;
	Bloom* bloom;
	AmbientTextures* ambTex;
};

