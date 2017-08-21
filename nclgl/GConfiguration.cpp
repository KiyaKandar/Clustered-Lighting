#include "GConfiguration.h"

#include "Renderer.h"

GConfiguration::GConfiguration(Renderer* renderer, Camera* camera, Vector2 resolution)
{
	this->resolution = resolution;
	this->renderer	 = renderer;
	this->camera	 = camera;

	settings.push_back(shadows);
	settings.push_back(SGBuffer);
	settings.push_back(ssao);
	settings.push_back(lighting);
	settings.push_back(bloom);
}

GConfiguration::~GConfiguration()
{
	delete ambTex;
	delete shadows;
	delete SGBuffer;
	delete ssao;
	delete lighting;
	delete bloom;
}

void GConfiguration::InitialiseSettings()
{
	//Initialise graphics components
	ambTex = new AmbientTextures();
	ambTex->textures = new GLuint*[1];
	ambTex->texUnits = new int[1];

	shadows = new Shadows(5, renderer->GetAllLights(), renderer->GetModels());
	shadows->LinkShaders();
	shadows->Initialise();

	SGBuffer = new GBuffer(camera, renderer->GetModelsInFrustum());
	SGBuffer->LinkShaders();
	SGBuffer->Initialise();

	ssao = new SSAO(camera, ambTex, SGBuffer->GetGBuffer());
	ssao->LinkShaders();
	ssao->Initialise();

	lighting = new BPLighting(camera, SGBuffer->GetGBuffer(),
		shadows->GetShadowData(), ambTex, 1);
	lighting->LinkShaders();
	lighting->Initialise();

	bloom = new Bloom(100);
	bloom->LinkShaders();
	bloom->Initialise();

	lighting->FBO = &bloom->FBO;
}

void GConfiguration::LinkToRenderer() 
{
	renderer->AddGSetting(shadows);
	renderer->AddGSetting(SGBuffer);
	renderer->AddGSetting(ssao);
	renderer->AddGSetting(lighting);
	renderer->AddGSetting(bloom);
}
