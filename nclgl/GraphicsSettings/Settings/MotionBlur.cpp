#include "MotionBlur.h"

#include "../Game/GraphicsConfiguration/GLConfig.h"
#include "../Game/GraphicsConfiguration/GLUtil.h"

MotionBlur::MotionBlur(GBufferData* gBuffer, 
	Matrix4* previousView, Matrix4* currentView, float* fps)
{
	blurShader = new Shader(SHADERDIR"/MotionBlur/combinevert.glsl", SHADERDIR"/MotionBlur/combinefrag.glsl");
	this->gBuffer = gBuffer;

	this->previousView = previousView;
	this->currentView = currentView;
	this->fps = fps;
}


MotionBlur::~MotionBlur()
{
	delete blurShader;
	glDeleteTextures(1, &colourBuffer[0]);
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

	glGenFramebuffers(1, &screenTexFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, screenTexFBO);

	glGenTextures(1, colourBuffer);
	GLUtil::CreateScreenTexture(colourBuffer[0], GL_RGB16F, GL_RGB, GL_FLOAT, GL_LINEAR, 0, true);

	glGenRenderbuffers(1, &renderBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, GLConfig::RESOLUTION.x, GLConfig::RESOLUTION.y);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderBuffer);

	glDrawBuffers(1, attachment);

	GLUtil::VerifyBuffer("Motion Blur Buffer", false);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	GLUtil::CheckGLError("Motion blur texture");
}

void MotionBlur::Apply()
{
	//glBindFramebuffer(GL_FRAMEBUFFER, screenTexFBO);
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
	glUniform1i(glGetUniformLocation(blurShader->GetProgram(), "scene"), 9);

	currentShader->ApplyTexture(GLConfig::GPOSITION, *gBuffer->gPosition);
	currentShader->ApplyTexture(9, colourBuffer[0]);

	RenderScreenQuad();
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
}