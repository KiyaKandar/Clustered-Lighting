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
	delete shadows;
	delete SGBuffer;
	delete ssao;
	delete lighting;
	delete bloom;
	delete motionBlur;
}

void GConfiguration::InitialiseSettings()
{
	//Initialise graphics components
	ambTex = new AmbientTextures();
	ambTex->textures = new GLuint*[1];
	ambTex->texUnits = new int[1];

	shadows = new Shadows(GLConfig::SHADOW_LIGHTS, renderer->GetAllLights(), renderer->GetModels());
	shadows->LinkShaders();
	shadows->Initialise();

	SGBuffer = new GBuffer(window, camera, renderer->GetModelsInFrustum(), renderer->GetModels());
	SGBuffer->LinkShaders();
	SGBuffer->Initialise();

	ssao = new SSAO(camera, ambTex, SGBuffer->GetGBuffer());
	ssao->LinkShaders();
	ssao->Initialise();

	lighting = new BPLighting(camera, SGBuffer->GetGBuffer(),
		shadows->GetShadowData(), ambTex, 1);
	lighting->LinkShaders();
	lighting->Initialise();


	bloom = new Bloom(GLConfig::BLOOM_STRENGTH);
	bloom->LinkShaders();
	bloom->Initialise();

	motionBlur = new MotionBlur(SGBuffer->GetGBuffer(), &renderer->previousViewMatrix,
		&renderer->currentViewProj, &profiler->GetFPSCounter()->fps);
	motionBlur->LinkShaders();
	motionBlur->Initialise();
	//motionBlur->FBO = &bloom->FBO;

	lighting->FBO = &bloom->FBO;
	bloom->motionBlurFBO = &motionBlur->screenTexFBO;
}

void GConfiguration::LinkToRenderer()
{
	renderer->AddGSetting(shadows);
	renderer->AddGSetting(SGBuffer);
	renderer->AddGSetting(ssao);
	renderer->AddGSetting(lighting);
	renderer->AddGSetting(bloom);
	renderer->AddGSetting(motionBlur);
}
