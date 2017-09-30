#pragma once

#include "OGLRenderer.h"
#include "Camera.h"
#include "SceneNode.h"
#include "Frustum.h"
#include "Mesh.h"
#include "../Game/Subsystem.h"
#include "Text.h"
#include "TextMesh.h"
#include "Model.h"
#include "GSetting.h"
#include "TileRenderer.h"

#include <algorithm> //For std::sort
#include <vector>
#include <random>
#include <functional>
#include "TextRenderer.h"

#define DEBUG_LIGHTS

const int NUM_LIGHTS = 100;
const int SHADOW_SIZE = 4096;
const int RESX = 1280;
const int RESY = 720;

const int GLOBAL_LIGHT = 0;

class Renderer : public OGLRenderer, public Subsystem
{
public:
	Renderer(Window &parent, Camera* cam);
	~Renderer();

	void Update(float deltatime) override;

	void UpdateScene(float msec) override;
	void RenderScene() override;

	void AddModel(Model* model) noexcept
	{
		models.push_back(model);
	}

	std::vector<ModelMesh*>* GetModelsInFrustum() noexcept
	{
		return &modelsInFrame;
	}

	std::vector<Model*>* GetModels() noexcept
	{
		return &models;
	}

	void AddGSetting(GSetting* component) noexcept
	{
		GComponents.push_back(component);
	}

	Light* GetGlobalLight()
	{
		return lights[GLOBAL_LIGHT];
	}

	Light** GetAllLights() 
	{
		return lights;
	}

	void SetCamera(Camera* cam)
	{
		camera = cam;
	}

	inline void SwitchToPerspective()
	{
		projMatrix = Matrix4::Perspective(1.0f, 15000.0f,
			static_cast<float>(width) / static_cast<float>(height), 45.0f);
	}

	inline void SwitchToOrthographic()
	{
		projMatrix = Matrix4::Orthographic(-1.0f, 10000.0f,
			width / 2.0f, -width / 2.0f, height / 2.0f,
			-height / 2.0f);
	}

	const float GetWidth() const
	{
		return static_cast<float>(width);
	}

	const float GetHeight() const
	{
		return static_cast<float>(height);
	}

	Camera* GetCamera() const
	{
		return camera;
	}

	void AddText(const Text& text) const
	{
		textRenderer->textbuffer.push_back(text);
	}

	//vector<Text> textbuffer;
	Font* basicFont;

protected:
	std::vector<GSetting*> GComponents;

	std::vector<Model*> models;
	std::vector<ModelMesh*> modelsInFrame;

	TileData* tileData;
	Tile* screenTiles;
	TileRenderer* tiles;

	bool debugMode = false;
	void InitDebugLights();

	//Light SSBO
	GLuint ssbo;
	GLuint tilesssbo;
	GLuint tilelightssssbo;
	void InitLightSSBO();

	Light* lights[NUM_LIGHTS];
	LightData lightData[NUM_LIGHTS];

	//Frustum culling + ordering.
	void BuildMeshLists();
	void SortMeshLists();
	void ClearMeshLists();

	//Handy debug stuff
	void DrawDebugLights();
	void RelinkShaders() const;
	std::vector<Model*> debugSpheres;

	//Text Stuff - Mainly for profiler
	Shader* textShader;
	void DrawAllText() const;
	TextRenderer* textRenderer;

	//Other Stuff
	Camera*	camera;
	Frustum frameFrustum;

	Window* wparent;
};
