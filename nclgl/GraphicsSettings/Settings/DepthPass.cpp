#include "DepthPass.h"

#include "../../../Game/GraphicsConfiguration/GLUtil.h"

DepthPass::DepthPass(Camera* camera, std::vector<ModelMesh*>* modelsInFrame)
{
	this->modelsInFrame = modelsInFrame;
	this->camera = camera;

	depthPassShader = new Shader(SHADERDIR"/depthPass_vert.glsl", SHADERDIR"/depthPass_frag.glsl");
}


DepthPass::~DepthPass()
{
	delete depthPassShader;
	glDeleteTextures(1, &depthTexture);
}

void DepthPass::LinkShaders()
{
	depthPassShader->LinkProgram();
}

void DepthPass::RegenerateShaders()
{
	depthPassShader->Regenerate();
}

void DepthPass::Initialise()
{
	GLUtil::ClearGLErrorStack();

	glGenFramebuffers(1, &depthFrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, depthFrameBuffer);

	glGenRenderbuffers(1, &depthTexture);
	glBindRenderbuffer(GL_RENDERBUFFER, depthTexture);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, GLConfig::RESOLUTION.x, GLConfig::RESOLUTION.y);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthTexture);

	GLUtil::VerifyBuffer("Depth Pass Render Buffer", false);
}

void DepthPass::Apply()
{
	glBindFramebuffer(GL_FRAMEBUFFER, depthFrameBuffer);
	glClear(GL_DEPTH_BUFFER_BIT);

	RenderGeometry();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void DepthPass::RenderGeometry()
{
	SetCurrentShader(depthPassShader);
	viewMatrix = camera->BuildViewMatrix();
	UpdateShaderMatrices();

	for (int i = 0; i < modelsInFrame->size(); ++i)
	{
		modelsInFrame->at(i)->Draw(*currentShader);
	}
}

void DepthPass::LocateUniforms()
{
}
