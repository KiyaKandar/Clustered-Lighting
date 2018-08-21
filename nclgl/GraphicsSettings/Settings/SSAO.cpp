#include "SSAO.h"

#include "../Game/GraphicsConfiguration/GLConfig.h"
#include "../Game/GraphicsConfiguration/GLUtil.h"

const int KERNEL_SIZE = 64;
const int RESOLUTION_SCALE_X = 320;
const int RESOLUTION_SCALE_Y = 180;

SSAO::SSAO(Camera* cam, AmbientTextures* ambientTextures, GBufferData* SGBuffer)
{
	this->ambientTextures = ambientTextures;
	this->SGBuffer = SGBuffer;
	camera = cam;

	//SSAO Shaders
	SSAOCol = new Shader(SHADERDIR"/SSAO/ssao_vert.glsl", SHADERDIR"/SSAO/ssao_frag.glsl");
	SSAOBlur = new Shader(SHADERDIR"/SSAO/ssao_vert.glsl", SHADERDIR"/SSAO/ssao_blurfrag.glsl");

	ambientTextures->textures[GLConfig::SSAO_INDEX] = &ssaoColorBufferBlur;
	ambientTextures->texUnits[GLConfig::SSAO_INDEX] = 6;

	xSize = GLConfig::RESOLUTION.x / RESOLUTION_SCALE_X;
	ySize = GLConfig::RESOLUTION.y / RESOLUTION_SCALE_Y;
}

void SSAO::LinkShaders()
{
	SSAOCol->LinkProgram();
	SSAOBlur->LinkProgram();
}

void SSAO::Initialise()
{
	InitSSAOBuffers();

	//For the SSAO texture
	GenerateSampleKernel();
	GenerateNoiseTexture();

	LocateUniforms();
}

void SSAO::LocateUniforms()
{
	loc_ssaoRadius = glGetUniformLocation(SSAOCol->GetProgram(), "radius");
	loc_ssaoBias = glGetUniformLocation(SSAOCol->GetProgram(), "bias");
	loc_gPosition = glGetUniformLocation(SSAOCol->GetProgram(), "gPosition");
	loc_gNormal = glGetUniformLocation(SSAOCol->GetProgram(), "gNormal");
	loc_texNoise = glGetUniformLocation(SSAOCol->GetProgram(), "texNoise");

	loc_kernel = glGetUniformLocation(SSAOCol->GetProgram(), "samples");

	loc_ssaoInput = glGetUniformLocation(SSAOBlur->GetProgram(), "ssaoInput");
	loc_xSize = glGetUniformLocation(SSAOBlur->GetProgram(), "xSize");
	loc_ySize = glGetUniformLocation(SSAOBlur->GetProgram(), "ySize");
}

void SSAO::Apply()
{
	glDepthMask(GL_FALSE);
	//Generate the SSAO texture
	GenerateSSAOTex();

	//Blur the texture
	SSAOBlurTex();
	glDepthMask(GL_TRUE);
}

void SSAO::RegenerateShaders()
{
	SSAOCol->Regenerate();
	SSAOBlur->Regenerate();
}

void SSAO::InitSSAOBuffers()
{
	//Create framebuffer to hold SSAO processing stage 
	glGenFramebuffers(1, &ssaoFBO);
	glGenFramebuffers(1, &ssaoBlurFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);

	//SSAO color buffer
	glGenTextures(1, &ssaoColorBuffer);
	GLUtil::CreateScreenTexture(ssaoColorBuffer, GL_RED, GL_RGB, GL_FLOAT, GL_NEAREST, 0, false);

	GLUtil::VerifyBuffer("SSAO Frame", false);

	//Blur stage
	glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
	glGenTextures(1, &ssaoColorBufferBlur);
	GLUtil::CreateScreenTexture(ssaoColorBufferBlur, GL_RED, GL_RGB, GL_FLOAT, GL_NEAREST, 0, false);

	GLUtil::VerifyBuffer("Blur", false);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SSAO::GenerateSampleKernel()
{
	const std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0);
	std::default_random_engine generator;

	for (unsigned int i = 0; i < KERNEL_SIZE; ++i)
	{
		Vector3 sample(randomFloats(generator) * 2.0 - 1.0,
			randomFloats(generator) * 2.0 - 1.0, randomFloats(generator));

		sample.Normalise();
		sample = sample * randomFloats(generator);
		float scale = float(i) / KERNEL_SIZE;

		//Scale samples so they're more aligned to center of kernel
		scale = lerp(0.1f, 1.0f, scale * scale);//0.1f + (scale * scale) * (1.0f - 0.1f); //Lerp
		sample = sample * scale;
		ssaoKernel.push_back(sample);
	}
}

void SSAO::GenerateNoiseTexture()
{
	const std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0);
	std::default_random_engine generator;

	//const int xSize = GLConfig::RESOLUTION.x / 640;
	//const int ySize = GLConfig::RESOLUTION.y / 360;

	const int noiseSize = (xSize * 2) * (ySize * 2);

	//Generate the texture
	for (unsigned int i = 0; i < noiseSize; i++)
	{
		Vector3 noise(
			randomFloats(generator) * 2.0 - 1.0,
			randomFloats(generator) * 2.0 - 1.0,
			0.0f); // rotate around z-axis (in tangent space)

		ssaoNoise.push_back(noise);
	}

	glGenTextures(1, &noiseTexture);
	glBindTexture(GL_TEXTURE_2D, noiseTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, xSize * 2, ySize * 2, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void SSAO::GenerateSSAOTex()
{
	glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
	glClear(GL_COLOR_BUFFER_BIT);

	SetCurrentShader(SSAOCol);

	glUniform3fv(loc_kernel, KERNEL_SIZE, (float*)&ssaoKernel[0]);

	viewMatrix = camera->BuildViewMatrix();

	//Basic uniforms
	UpdateShaderMatrices();

	glUniform1i(loc_ssaoRadius, ssaoRadius);
	glUniform1i(loc_ssaoBias, ssaoBias);

	//Texture units
	glUniform1i(loc_gPosition, GLConfig::GPOSITION);
	glUniform1i(loc_gNormal, GLConfig::GNORMAL);
	glUniform1i(loc_texNoise, NOISE_TEX);

	glUniform1i(glGetUniformLocation(SSAOCol->GetProgram(), "xSize"), xSize);
	glUniform1i(glGetUniformLocation(SSAOCol->GetProgram(), "ySize"), ySize);

	currentShader->ApplyTexture(GLConfig::GPOSITION, *SGBuffer->gPosition);
	currentShader->ApplyTexture(GLConfig::GNORMAL, *SGBuffer->gNormal);
	currentShader->ApplyTexture(NOISE_TEX, noiseTexture);

	RenderScreenQuad();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SSAO::SSAOBlurTex()
{
	glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
	glClear(GL_COLOR_BUFFER_BIT);

	SetCurrentShader(SSAOBlur);

	glUniform1i(loc_xSize, xSize);
	glUniform1i(loc_ySize, ySize);

	glUniform1i(loc_ssaoInput, SSAO_TEX);
	currentShader->ApplyTexture(SSAO_TEX, ssaoColorBuffer);

	RenderScreenQuad();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}