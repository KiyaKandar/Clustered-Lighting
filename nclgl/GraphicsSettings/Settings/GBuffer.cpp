#include "GBuffer.h"

#include "../Game/GraphicsConfiguration/GLConfig.h"
#include "../Game/GraphicsConfiguration/GLUtil.h"

GBuffer::GBuffer(Window* window, Camera* camera, std::vector<ModelMesh*>* modelsInFrame,
	std::vector<Model*>** models)
{
	this->modelsInFrame = modelsInFrame;
	this->models = models;
	this->camera = camera;
	this->window = window;

	geometryPass = new Shader(SHADERDIR"/SSAO/ssao_geometryvert.glsl",
		SHADERDIR"/SSAO/ssao_geometryfrag.glsl", "", true);

	SGBuffer = new GBufferData();
	SGBuffer->gAlbedo = &gAlbedo;
	SGBuffer->gNormal = &gNormal;
	SGBuffer->gPosition = &gPosition;
}

GBuffer::~GBuffer()
{
	delete geometryPass;
}

void GBuffer::LinkShaders()
{
	geometryPass->LinkProgram();
}

void GBuffer::RegenerateShaders()
{
	geometryPass->Regenerate();
}

void GBuffer::Initialise()
{
	InitGBuffer();
	InitAttachments();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GBuffer::Apply()
{
	//Render any geometry to GBuffer
	FillGBuffer();
}

void GBuffer::InitGBuffer()
{
	GLUtil::ClearGLErrorStack();

	glGenFramebuffers(1, &gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

	//Position colour buffer
	glGenTextures(1, &gPosition);
	GLUtil::CreateScreenTexture(gPosition, GL_RGB16F, GL_RGB, GL_FLOAT, GL_NEAREST, GLConfig::GPOSITION, true);
	GLUtil::CheckGLError("GPosition");

	//Normal coluor buffer
	glGenTextures(1, &gNormal);
	GLUtil::CreateScreenTexture(gNormal, GL_RGB16F, GL_RGB, GL_FLOAT, GL_NEAREST, GLConfig::GNORMAL, false);
	GLUtil::CheckGLError("GNormal");

	//Colour + specular colour buffer
	glGenTextures(1, &gAlbedo);
	GLUtil::CreateScreenTexture(gAlbedo, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, GL_NEAREST, GLConfig::GALBEDO, false);
	GLUtil::CheckGLError("GAlbedo");

	GLUtil::VerifyBuffer("GBuffer", false);
}

void GBuffer::InitAttachments()
{
	//Tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
	glDrawBuffers(3, attachments);

	//Create and attach depth buffer (renderbuffer)
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, GLConfig::RESOLUTION.x, GLConfig::RESOLUTION.y);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

	GLUtil::VerifyBuffer("RBO Depth GBuffer", false);
}

void GBuffer::FillGBuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	SetCurrentShader(geometryPass);

	viewMatrix = camera->BuildViewMatrix();
	UpdateShaderMatrices();

	const int numModels = modelsInFrame->size();
	for (int i = 0; i < numModels; ++i)
	{
		glUniform1i(glGetUniformLocation(geometryPass->GetProgram(), "hasTexture"), modelsInFrame->at(i)->hasTexture);
		modelsInFrame->at(i)->Draw(*currentShader);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
