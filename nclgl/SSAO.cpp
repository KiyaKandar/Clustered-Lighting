#include "SSAO.h"

SSAO::SSAO(Vector2 resolution, Camera* cam, 
	AmbientTextures* ambientTextures, GBufferData* SGBuffer) : GSetting(resolution)
{
	this->ambientTextures	= ambientTextures;
	this->SGBuffer			= SGBuffer;
	camera = cam;

	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)resolution.x / (float)resolution.y, 45.0f);

	//SSAO Shaders
	SSAOCol		 = new Shader(SHADERDIR"/SSAO/ssao_vert.glsl",			SHADERDIR"/SSAO/ssao_frag.glsl");
	SSAOBlur	 = new Shader(SHADERDIR"/SSAO/ssao_vert.glsl",			SHADERDIR"/SSAO/ssao_blurfrag.glsl");

	ambientTextures->textures[SSAO_INDEX] = &ssaoColorBufferBlur;
	ambientTextures->texUnits[SSAO_INDEX] = 3;
}

void SSAO::LinkShaders()
{
	SSAOCol		->LinkProgram();
	SSAOBlur	->LinkProgram();
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
	loc_ssaoRadius	= glGetUniformLocation(SSAOCol->GetProgram(), "radius");
	loc_ssaoBias	= glGetUniformLocation(SSAOCol->GetProgram(), "bias");
	loc_gPosition	= glGetUniformLocation(SSAOCol->GetProgram(), "gPosition");
	loc_gNormal		= glGetUniformLocation(SSAOCol->GetProgram(), "gNormal");
	loc_texNoise	= glGetUniformLocation(SSAOCol->GetProgram(), "texNoise");
	loc_ssaoInput	= glGetUniformLocation(SSAOBlur->GetProgram(), "ssaoInput");
}	

void SSAO::Apply()
{
	//Generate the SSAO texture
	GenerateSSAOTex();

	//Blur the texture
	SSAOBlurTex();
}

void SSAO::RegenerateShaders()
{
	SSAOCol		->Regenerate();
	SSAOBlur	->Regenerate();
}

void SSAO::InitSSAOBuffers()
{
	//Create framebuffer to hold SSAO processing stage 
	glGenFramebuffers(1, &ssaoFBO);
	glGenFramebuffers(1, &ssaoBlurFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);

	//SSAO color buffer
	glGenTextures(1, &ssaoColorBuffer);
	glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, resolution.x, resolution.y, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBuffer, 0);

	CheckBuffer("SSAO Frame");

	//Blur stage
	glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
	glGenTextures(1, &ssaoColorBufferBlur);
	glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, resolution.x, resolution.y, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBufferBlur, 0);

	CheckBuffer("Blur");
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SSAO::GenerateSampleKernel()
{
	std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0);
	std::default_random_engine generator;

	for (unsigned int i = 0; i < 64; ++i)
	{
		Vector3 sample(randomFloats(generator) * 2.0 - 1.0,
			randomFloats(generator) * 2.0 - 1.0, randomFloats(generator));

		sample.Normalise();
		sample = sample * randomFloats(generator);
		float scale = float(i) / 64;

		//Scale samples so they're more aligned to center of kernel
		scale = lerp(0.1f, 1.0f, scale * scale);//0.1f + (scale * scale) * (1.0f - 0.1f); //Lerp
		sample = sample * scale;
		ssaoKernel.push_back(sample);
	}
}

void SSAO::GenerateNoiseTexture()
{
	std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0);
	std::default_random_engine generator;

	//Generate the texture
	for (unsigned int i = 0; i < 16; i++)
	{
		Vector3 noise(
			randomFloats(generator) * 2.0 - 1.0,
			randomFloats(generator) * 2.0 - 1.0, 
			0.0f); // rotate around z-axis (in tangent space)

		ssaoNoise.push_back(noise);
	}

	glGenTextures(1, &noiseTexture);
	glBindTexture(GL_TEXTURE_2D, noiseTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
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

	//Send kernel + rotation 
	for (unsigned int i = 0; i < 64; ++i)
	{
		currentShader->SetVec3("samples[" + std::to_string(i) + "]", ssaoKernel[i]);
	}

	viewMatrix = camera->BuildViewMatrix();

	//Basic uniforms
	UpdateShaderMatrices();

	glUniform1i(loc_ssaoRadius, ssaoRadius);
	glUniform1i(loc_ssaoBias, ssaoBias);

	//Texture units
	glUniform1i(loc_gPosition,	GPOSITION);
	glUniform1i(loc_gNormal,	GNORMAL);
	glUniform1i(loc_texNoise,	NOISE_TEX);

	currentShader->ApplyTexture(GPOSITION, *SGBuffer->gPosition);
	currentShader->ApplyTexture(GNORMAL, *SGBuffer->gNormal);
	currentShader->ApplyTexture(NOISE_TEX, noiseTexture);

	RenderScreenQuad();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SSAO::SSAOBlurTex()
{
	glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
	glClear(GL_COLOR_BUFFER_BIT);

	SetCurrentShader(SSAOBlur);

	glUniform1i(loc_ssaoInput, SSAO_TEX);
	currentShader->ApplyTexture(SSAO_TEX, ssaoColorBuffer);

	RenderScreenQuad();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}