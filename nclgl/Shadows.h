#pragma once

#include "GSetting.h"

//#define NUM_LIGHTS 5
#define SHADOWSIZE 4096

#include "Model.h"

#include <vector>

class Shadows : public GSetting
{
public:
	Shadows(int numShadowCastingLights, Light** lights, std::vector<Model*>* models);

	~Shadows()
	{
		delete shadowShader;
		delete shadowData;
		delete shadowFBOs;
	}

	void LinkShaders();
	void Initialise();
	void Apply();

	void RegenerateShaders();

	ShadowData* GetShadowData()
	{
		return shadowData;
	}

private:
	void LocateUniforms() {}

	//Shadow prep
	void InitShadowTex();
	void InitShadowBuffer();

	//Application
	void DrawShadowScene();

	GLuint* shadowFBOs;
	ShadowData* shadowData;
	Shader* shadowShader;

	Light** lights;
	std::vector<Model*>* models;
};

