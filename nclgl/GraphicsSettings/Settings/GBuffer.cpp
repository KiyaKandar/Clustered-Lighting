#include "GBuffer.h"

#include "../Game/GraphicsConfiguration/GLConfig.h"
#include "../Game/GraphicsConfiguration/GLUtil.h"

GBuffer::GBuffer(Window* window, Camera* camera, std::vector<ModelMesh*>* modelsInFrame,
	vector<ModelMesh*>* transparentModelsInFrame, std::vector<Model*>** models)
{
	this->modelsInFrame = modelsInFrame;
	this->transparentModelsInFrame = transparentModelsInFrame;
	this->models = models;
	this->camera = camera;
	this->window = window;

	geometryPass = new Shader(SHADERDIR"/SSAO/ssao_geometryvert.glsl",
		SHADERDIR"/SSAO/ssao_geometryfrag.glsl", "", true);

	SGBuffer = new GBufferData();
	SGBuffer->gAlbedo = &gAlbedo;
	SGBuffer->gNormal = &gNormal;
	SGBuffer->gPosition = &gPosition;
	SGBuffer->gMetallic = &gMetallic;
	SGBuffer->gRoughness = &gRoughness;
}

GBuffer::~GBuffer()
{
	delete geometryPass;
	glDeleteTextures(1, &gPosition);
	glDeleteTextures(1, &gNormal);
	glDeleteTextures(1, &gAlbedo);
	glDeleteTextures(1, &gMetallic);
	glDeleteTextures(1, &gRoughness);
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
	LocateUniforms();
	glEnable(GL_DEPTH_TEST);
	glCullFace(GL_BACK);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GBuffer::Apply()
{
	//Render any geometry to GBuffer
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	RenderGeometry(modelsInFrame);

	glEnable(GL_BLEND);

	RenderGeometry(transparentModelsInFrame);

	glDisable(GL_BLEND);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void GBuffer::LocateUniforms()
{
	SetCurrentShader(geometryPass);

	loc_skybox = glGetUniformLocation(geometryPass->GetProgram(), "skybox");
	loc_cameraPos = glGetUniformLocation(geometryPass->GetProgram(), "cameraPos");
	loc_hasTexture = glGetUniformLocation(geometryPass->GetProgram(), "hasTexture");
	loc_isReflective = glGetUniformLocation(geometryPass->GetProgram(), "isReflective");
	loc_reflectionStrength = glGetUniformLocation(geometryPass->GetProgram(), "reflectionStrength");
	loc_baseColour = glGetUniformLocation(geometryPass->GetProgram(), "baseColour");
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

	glGenTextures(1, &gMetallic);
	GLUtil::CreateScreenTexture(gMetallic, GL_RGB16F, GL_RGB, GL_FLOAT, GL_NEAREST, GLConfig::GMETALLIC, false);
	GLUtil::CheckGLError("GMetallic");

	glGenTextures(1, &gRoughness);
	GLUtil::CreateScreenTexture(gRoughness, GL_RGB16F, GL_RGB, GL_FLOAT, GL_NEAREST, GLConfig::GROUGHNESS, false);
	GLUtil::CheckGLError("GRoughness");

	GLUtil::VerifyBuffer("GBuffer", false);
}

void GBuffer::InitAttachments()
{
	//Tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
	glDrawBuffers(5, attachments);

	//Create and attach depth buffer (renderbuffer)
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, GLConfig::RESOLUTION.x, GLConfig::RESOLUTION.y);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

	GLUtil::VerifyBuffer("RBO Depth GBuffer", false);
}

void GBuffer::RenderGeometry(vector<ModelMesh*>* meshes)
{
	SetCurrentShader(geometryPass);
	viewMatrix = camera->BuildViewMatrix();
	UpdateShaderMatrices();

	glActiveTexture(GL_TEXTURE0);
	glUniform1i(loc_skybox, 0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	glUniform3fv(loc_cameraPos, 1, (float*)&camera->GetPosition());

	for (int i = 0; i < meshes->size(); ++i)
	{
		glUniform1i(loc_hasTexture, meshes->at(i)->hasTexture);
		glUniform1i(loc_isReflective, meshes->at(i)->isReflective);
		glUniform1f(loc_reflectionStrength, meshes->at(i)->reflectionStrength);
		glUniform4fv(loc_baseColour, 1, (float*)&meshes->at(i)->baseColour);
		//glUniform1f(glGetUniformLocation(geometryPass->GetProgram(), "customMetallic"), meshes->at(i)->metallic);
		//glUniform1f(glGetUniformLocation(geometryPass->GetProgram(), "customRoughness"), meshes->at(i)->roughness);

		meshes->at(i)->Draw(*currentShader);
	}
}
