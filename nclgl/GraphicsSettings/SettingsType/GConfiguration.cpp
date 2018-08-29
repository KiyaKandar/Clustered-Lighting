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
	delete lighting;
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

	lighting = new BPLighting(camera, SGBuffer->GetGBuffer(), ambTex, 1, window);
	lighting->LinkShaders();
	lighting->Initialise();

	renderer->gBuffer = SGBuffer;
	renderer->lighting = lighting;
}

void GConfiguration::LinkToRenderer()
{
	renderer->AddGSetting(SGBuffer);
	renderer->AddGSetting(lighting);
}
