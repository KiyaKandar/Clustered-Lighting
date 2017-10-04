#include "Shadows.h"

#include "../Game/GraphicsConfiguration/GLConfig.h"
#include "../Game/GraphicsConfiguration/GLUtil.h"

Shadows::Shadows(int numShadowCastingLights, Light** lights,
	std::vector<Model*>* models)
{
	shadowFBOs = new GLuint[numShadowCastingLights];

	shadowData = new ShadowData();
	shadowData->NUM_LIGHTS = numShadowCastingLights;

	shadowData->shadowIndexes = new int[numShadowCastingLights];
	shadowData->shadows = new GLuint[numShadowCastingLights];
	shadowData->textureMatrices = new Matrix4[numShadowCastingLights];

	for (int i = 0; i < numShadowCastingLights; i++)
	{
		shadowData->shadowIndexes[i] = i + 4;
	}

	shadowShader = new Shader(SHADERDIR"shadowvert.glsl", SHADERDIR"shadowfrag.glsl");

	this->lights = lights;
	this->models = models;
}

void Shadows::LinkShaders()
{
	shadowShader->LinkProgram();
}

void Shadows::RegenerateShaders()
{
	shadowShader->Regenerate();
}

void Shadows::Initialise()
{
	//Shadow prep
	InitShadowTex();
	InitShadowBuffer();
}

void Shadows::Apply()
{
	DrawShadowScene();
}

void Shadows::InitShadowTex()
{
	for (int i = 0; i < shadowData->NUM_LIGHTS; ++i)
	{
		glGenTextures(1, &shadowData->shadows[i]);
		glBindTexture(GL_TEXTURE_2D, shadowData->shadows[i]);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
			SHADOWSIZE, SHADOWSIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE,
			GL_COMPARE_R_TO_TEXTURE);

		glBindTexture(GL_TEXTURE_2D, 0);
	}

	GLUtil::CheckGLError("Shadow textures");
}

void Shadows::InitShadowBuffer()
{
	for (int i = 0; i < shadowData->NUM_LIGHTS; ++i)
	{
		glGenFramebuffers(1, &shadowFBOs[i]);
		glBindFramebuffer(GL_FRAMEBUFFER, shadowFBOs[i]);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
			GL_TEXTURE_2D, shadowData->shadows[i], 0);

		glDrawBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		GLUtil::VerifyBuffer("Shadow Buffer " + i, false);
	}
}

void Shadows::DrawShadowScene()
{
	SetCurrentShader(shadowShader);

	for (int i = 0; i < shadowData->NUM_LIGHTS; ++i)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, shadowFBOs[i]);

		glClear(GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, SHADOWSIZE, SHADOWSIZE);
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

		viewMatrix = Matrix4::BuildViewMatrix(lights[i]->GetPosition(), Vector3(0, 0, 0));
		shadowData->textureMatrices[i] = biasMatrix * (projMatrix * viewMatrix);

		UpdateShaderMatrices();

		for each (Model* m in *models)
		{
			for each (ModelMesh* mesh in m->meshes)
			{
				mesh->DrawShadow(*currentShader);
			}
		}
	}

	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glViewport(0, 0, GLConfig::RESOLUTION.x, GLConfig::RESOLUTION.y);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}