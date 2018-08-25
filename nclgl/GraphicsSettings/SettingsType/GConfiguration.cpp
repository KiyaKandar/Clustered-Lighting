#include "GConfiguration.h"

#include "../../Rendering/Renderer/Renderer.h"
#include "../Game/GraphicsConfiguration/GLConfig.h"

GConfiguration::GConfiguration(Window* window, Renderer* renderer, Camera* camera, Vector2 resolution, Profiler* profiler)
{
	this->resolution = resolution;
	this->renderer = renderer;
	this->camera = camera;
	this->profiler = profiler;
	this->window = window;
}

GConfiguration::~GConfiguration()
{
	delete ambTex;
	delete SGBuffer;
	delete ssao;
	delete lighting;
	delete bloom;
	delete motionBlur;
	delete skybox;
	delete particles;
}

void GConfiguration::InitialiseSettings()
{
	//Initialise graphics components
	ambTex = new AmbientTextures();
	ambTex->textures = new GLuint*[1];
	ambTex->texUnits = new int[1];

	SGBuffer = new GBuffer(window, camera, renderer->GetModelsInFrustum(), renderer->GetTransparentModelsInFrustum(), renderer->GetModels());
	SGBuffer->LinkShaders();
	SGBuffer->Initialise();

	ssao = new SSAO(camera, ambTex, SGBuffer->GetGBuffer());
	ssao->LinkShaders();
	ssao->Initialise();

	lighting = new BPLighting(camera, SGBuffer->GetGBuffer(), ambTex, 1, window);
	lighting->LinkShaders();
	lighting->Initialise();

	bloom = new Bloom(GLConfig::BLOOM_STRENGTH);
	bloom->LinkShaders();
	bloom->Initialise();

	motionBlur = new MotionBlur(SGBuffer->GetGBuffer(), &renderer->previousViewMatrix,
		&renderer->currentViewProj, &profiler->GetFPSCounter()->fps);
	motionBlur->LinkShaders();
	motionBlur->Initialise();

	skybox = new Skybox(&camera->viewMatrix);
	skybox->LinkShaders();
	skybox->Initialise();
	skybox->GBufferFBO = &SGBuffer->gBuffer;

	particles = new ParticleSystem(&camera->viewMatrix);
	particles->LinkShaders();
	particles->Initialise();
	particles->motionBlurFBO = &motionBlur->screenTexFBO;

	renderer->gBuffer = SGBuffer;
	renderer->skybox = skybox;

	lighting->FBO = &bloom->FBO;
	bloom->motionBlurFBO = &motionBlur->screenTexFBO;

	SGBuffer->skybox = skybox;
	renderer->lighting = lighting;
	renderer->particleSystem = particles;
}

void GConfiguration::LinkToRenderer()
{
	renderer->AddGSetting(SGBuffer);
	renderer->AddGSetting(ssao);
	renderer->AddGSetting(lighting);
	renderer->AddGSetting(bloom);
	renderer->AddGSetting(particles);
	renderer->AddGSetting(motionBlur);
}
