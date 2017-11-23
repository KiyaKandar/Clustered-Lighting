#pragma once

#include "../SettingsType/GSetting.h"

#include "../../Rendering/View/Camera.h"
#include "../../ASSIMP/ModelMesh.h"

#include <vector>
#include <string>
#include <random>

const int NOISE_TEX = 2;
const int SSAO_TEX = 0;

class SSAO : public GSetting
{
public:
	SSAO(Camera* cam, AmbientTextures* ambientTextures, GBufferData* SGBuffer);

	virtual ~SSAO()
	{
		delete SSAOCol;
		delete SSAOBlur;
		delete SGBuffer;

		glDeleteTextures(1, &ssaoColorBuffer);
		glDeleteTextures(1, &ssaoColorBufferBlur);
	}

	void LinkShaders() override;
	void Initialise() override;
	void Apply() override;

	void RegenerateShaders() override;

	void SetViewMatrix(Matrix4 mat)
	{
		viewMatrix = mat;
	}

	float lerp(float a, float b, float f)
	{
		return a + f * (b - a);
	}

private:
	void LocateUniforms() override;

	Camera* camera;
	AmbientTextures* ambientTextures;
	GBufferData* SGBuffer;

	//Init Functions
	void InitSSAOBuffers();
	void GenerateSampleKernel();
	void GenerateNoiseTexture();

	/*
	  Render Functions.
	  MUST TAKE PLACE IN THIS ORDER.
	*/
	void GenerateSSAOTex();
	void SSAOBlurTex();

	//SSAO Vars
	GLuint ssaoFBO;
	GLuint ssaoBlurFBO;
	GLuint noiseTexture;
	GLuint ssaoColorBuffer, ssaoColorBufferBlur;

	//Uniform locations
	GLint loc_ssaoRadius;
	GLint loc_ssaoBias;
	GLint loc_gPosition;
	GLint loc_gNormal;
	GLint loc_texNoise;
	GLint loc_ssaoInput;
	GLint loc_xSize;
	GLint loc_ySize;

	GLuint loc_kernel;

	std::vector<Vector3> ssaoKernel;
	std::vector<Vector3> ssaoNoise;

	//SSAO Uniforms
	float ssaoRadius = 0.4;
	float ssaoBias = 0.020;

	//SSAO Shaders
	Shader* SSAOCol;
	Shader* SSAOBlur;

	int xSize;
	int ySize;
};

