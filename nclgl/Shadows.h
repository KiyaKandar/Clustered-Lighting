#pragma once

#include "GSetting.h"

//#define NUM_LIGHTS 5
#define SHADOWSIZE 4096

#include "Model.h"

#include <vector>

class Shadows : public GSetting
{
public:
	Shadows(int numShadowCastingLights, Vector2 resolution, Light** lights, std::vector<Model*>* models);

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

	Light** lights;
	std::vector<Model*>* models;

	//Shadow prep
	void InitShadowTex();
	void InitShadowBuffer();

	//Application
	void DrawShadowScene();

	//Shadow vars
	//GLuint shadows[NUM_LIGHTS];
	GLuint* shadowFBOs;// [5];
	//Matrix4 textureMatrices[NUM_LIGHTS];
	//int* shadowIndexes;

	ShadowData* shadowData;

	Shader* shadowShader;
};

