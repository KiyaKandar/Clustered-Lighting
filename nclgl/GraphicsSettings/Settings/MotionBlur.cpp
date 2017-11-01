#include "MotionBlur.h"

#include "../Game/GraphicsConfiguration/GLConfig.h"
#include "../Game/GraphicsConfiguration/GLUtil.h"

MotionBlur::MotionBlur(GBufferData* gBuffer, 
	Matrix4* previousView, Matrix4* currentView, float* fps)
{
	blurShader = new Shader(SHADERDIR"/MotionBlur/combinevert.glsl", SHADERDIR"/MotionBlur/combinefrag.glsl", "", true);
	this->gBuffer = gBuffer;

	this->previousView = previousView;
	this->currentView = currentView;
	this->fps = fps;
}


MotionBlur::~MotionBlur()
{
	delete blurShader;
}

void MotionBlur::LinkShaders()
{
	blurShader->LinkProgram();
}

void MotionBlur::RegenerateShaders()
{
	blurShader->Regenerate();
}

void MotionBlur::Initialise()
{
	CreateTexture();
}

void MotionBlur::CreateTexture()
{
	GLUtil::ClearGLErrorStack();

	//glGenFramebuffers(1, FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, *FBO);

	glGenTextures(1, colourBuffer);
	GLUtil::CreateScreenTexture(colourBuffer[0], GL_RGB16F, GL_RGB, GL_FLOAT, GL_LINEAR, 2, true);

	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, GLConfig::RESOLUTION.x, GLConfig::RESOLUTION.y);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

	glDrawBuffers(1, attachment);

	GLUtil::VerifyBuffer("RBO DEPTH", false);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	GLUtil::CheckGLError("Bloom texture");
}

void MotionBlur::Apply()
{
	glEnable(GL_DEPTH_TEST);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	SetCurrentShader(blurShader);

	glUniformMatrix4fv(glGetUniformLocation(blurShader->GetProgram(), "projMtx"),
		1, false, (float*)&GLConfig::SHARED_PROJ_MATRIX);

	Matrix4 transformEyeSpace = *previousView * Matrix4::Inverse(*currentView);
	glUniformMatrix4fv(glGetUniformLocation(blurShader->GetProgram(), "transformEyeSpace"),
		1, false, (float*)&transformEyeSpace);

	glUniform1i(glGetUniformLocation(blurShader->GetProgram(), "fps"), static_cast<int>(*fps));
	glUniform1i(glGetUniformLocation(blurShader->GetProgram(), "gPosition"), GLConfig::GPOSITION);

	currentShader->ApplyTexture(GLConfig::GPOSITION, *gBuffer->gPosition);
	glUniform1i(glGetUniformLocation(blurShader->GetProgram(), "scene"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, colourBuffer[0]);

	RenderScreenQuad();
}