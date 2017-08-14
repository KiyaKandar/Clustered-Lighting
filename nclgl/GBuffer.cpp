#include "GBuffer.h"

GBuffer::GBuffer(Vector2 resolution, Camera* camera, 
	std::vector<ModelMesh*>* modelsInFrame) : GSetting(resolution)
{
	this->modelsInFrame = modelsInFrame;
	this->camera		= camera;

	geometryPass = new Shader(SHADERDIR"/SSAO/ssao_geometryvert.glsl", 
		SHADERDIR"/SSAO/ssao_geometryfrag.glsl");

	SGBuffer = new GBufferData();
	SGBuffer->gAlbedo	= &gAlbedo;
	SGBuffer->gNormal	= &gNormal;
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

	//External checks
	CheckBuffer("Frame");
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GBuffer::Apply()
{
	//Render any geometry to GBuffer
	FillGBuffer();
}

void GBuffer::InitGBuffer()
{
	glGenFramebuffers(1, &gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

	//Position colour buffer
	glGenTextures(1, &gPosition);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, resolution.x, resolution.y, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

	//Normal coluor buffer
	glGenTextures(1, &gNormal);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, resolution.x, resolution.y, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

	//Colour + specular colour buffer
	glGenTextures(1, &gAlbedo);
	glBindTexture(GL_TEXTURE_2D, gAlbedo);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, resolution.x, resolution.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedo, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << " g-buffer not complete!" << std::endl;
	}
}

void GBuffer::InitAttachments()
{
	//Tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
	glDrawBuffers(3, attachments);

	//Create and attach depth buffer (renderbuffer)
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, resolution.x, resolution.y);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);


}

void GBuffer::FillGBuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	SetCurrentShader(geometryPass);

	viewMatrix = camera->BuildViewMatrix();
	UpdateShaderMatrices();

	for (int i = 0; i < modelsInFrame->size(); ++i)
	{
		modelsInFrame->at(i)->Draw(*currentShader);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
