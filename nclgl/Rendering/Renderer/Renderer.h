#pragma once

#include "OGLRenderer.h"
#include "../View/Camera.h"
#include "../../Maths/Frustum.h"
#include "../Game/Subsystems/Subsystem.h"
#include "../Text/Text.h"
#include "../Text/TextMesh.h"
#include "../Text/TextRenderer.h"
#include "../../ASSIMP/Model.h"
#include "../../GraphicsSettings/SettingsType/GSetting.h"
#include "../ClusteredRendering/TileRenderer/TileRenderer.h"

#include <algorithm> //For std::sort
#include <vector>
#include <random>
#include <functional>
#include "../../../Game/SceneManagement/Scene.h"
#include "../../MD5/MD5FileData.h"
#include "../../MD5/MD5Node.h"
#include "../../Assets/Skybox.h"
#include "../../Assets/ParticleSystem.h"
#include "../../GraphicsSettings/Settings/GBuffer.h"
#include "../../GraphicsSettings/Settings/BPLighting.h"

#define DEBUG_LIGHTS

const int SHADOW_SIZE = 4096;
const int RESX = 1280;
const int RESY = 720;
const float MAX_MS_UNTIL_SCENE_CHANGE = 15000.0f;
const int GLOBAL_LIGHT = 0;

class Renderer : public OGLRenderer, public Subsystem
{
public:
	Renderer(Window &parent, Camera* cam);
	~Renderer();

	void Update(const float& deltatime) override;

	void UpdateScene(const float& msec) override;
	void RenderScene() override;

	void AddScene(Scene* scene) noexcept
	{
		scenes.push_back(scene);

		++sceneIndex;
		ChangeScene();
	}

	std::vector<ModelMesh*>* GetModelsInFrustum() noexcept
	{
		return &modelsInFrame;
	}

	vector<ModelMesh*>* GetTransparentModelsInFrustum()
	{
		return &transparentModelsInFrame;
	}

	std::vector<Model*>** GetModels() noexcept
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
		projMatrix = Matrix4::Perspective(1.0f, 4000.0f,
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

	void AddProfilerText(const Text& text) const
	{
		profilerTextRenderer->textbuffer.push_back(text);
	}

	void ChangeScene();

	Matrix4 previousViewMatrix;
	Matrix4 inverseViewProj;
	Matrix4 previousViewProj;
	Matrix4 currentViewProj;
	Vector3* currentScenesDataPrepWorkGroups;

	Skybox* skybox;
	GBuffer* gBuffer;
	BPLighting* lighting;
	ParticleSystem* particleSystem;

protected:
	std::vector<GSetting*> GComponents;
	std::vector<Scene*> scenes;
	vector<Model*>* models;
	std::vector<ModelMesh*> modelsInFrame;
	std::vector<ModelMesh*> transparentModelsInFrame;

	TileData* tileData;
	Tile* screenTiles;
	TileRenderer* tiles;

	bool debugMode = false;
	bool profilerEnabled = false;

	//Light SSBO
	GLuint ssbo;
	GLuint spotlightssbo;
	GLuint tilesssbo;
	GLuint tilelightssssbo;
	GLuint modelMatricesSSBO;
	void InitLightSSBO();

	Light* lights[GLConfig::NUM_LIGHTS];
	Light* defaultLights[GLConfig::NUM_LIGHTS];
	LightData lightData[GLConfig::NUM_LIGHTS];
	SpotLightData spotLightData[GLConfig::NUM_LIGHTS];

	//Frustum culling + ordering.
	void BuildMeshLists();
	void SortMeshLists();
	void ClearMeshLists();

	//Handy debug stuff
	void DrawDebugLights();
	void RelinkShaders() const;

	//Text Stuff - Mainly for profiler
	void DrawProfilerText() const;
	TextRenderer* profilerTextRenderer;

	//Other Stuff
	Camera*	camera;
	Frustum frameFrustum;
	Shader* textShader;

	Window* wparent;
	int sceneIndex;
	bool autoChangeScene = false;
	bool pauseAuto = false;
	float msUntilSceneChange = MAX_MS_UNTIL_SCENE_CHANGE;
};
