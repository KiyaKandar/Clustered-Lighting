#pragma once

#include "../SettingsType/GSetting.h"

//#define NUM_LIGHTS 5
#define SHADOWSIZE 4096

#include "../../ASSIMP/Model.h"

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

	void LinkShaders() override;
	void Initialise() override;
	void Apply() override;

	void RegenerateShaders() override;

	ShadowData* GetShadowData() const
	{
		return shadowData;
	}

private:
	void LocateUniforms() override
	{}

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

