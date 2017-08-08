#pragma once

#include "OGLRenderer.h"
#include "Camera.h"
#include "SceneNode.h"
#include "Frustum.h"
#include "OBJMesh.h"
#include "Mesh.h"
#include "../Game/Subsystem.h"
#include "Text.h"
#include "TextMesh.h"
#include "Model.h"
#include "GSetting.h"
#include "TileRenderer.h"

#include <algorithm> //For std::sort
#include <vector>
#include <unordered_map>
#include <random>

const int NUM_LIGHTS = 10;
#define SHADOWSIZE 4096
#define DEBUG_LIGHTS
#define RESX 1280
#define RESY 720

class Renderer : public OGLRenderer, public Subsystem
{
public:
	Renderer(Window &parent, Camera* cam);
	~Renderer(void);

	void Update(float deltatime);

	void UpdateScene(float msec);
	void RenderScene();

	void AddGSetting(GSetting* component)
	{
		GComponents.push_back(component);
	}

	Light** GetAllLights() 
	{
		return lights;
	}

	void SetCamera(Camera* cam)
	{
		camera = cam;
	}

	Light* GetLight()
	{
		return lights[0];
	}

	float GetWidth() const
	{
		return width;
	}

	inline void SwitchToPerspective()
	{
		projMatrix = Matrix4::Perspective(1.0f, 15000.0f,
			(float)width / (float)height, 45.0f);
	}

	inline void SwitchToOrthographic()
	{
		projMatrix = Matrix4::Orthographic(-1.0f, 10000.0f,
			width / 2.0f, -width / 2.0f, height / 2.0f,
			-height / 2.0f);
	}

	float lerp(float a, float b, float f)
	{
		return a + f * (b - a);
	}

	//bool TileIntersection(float radius, Vector2 lightCoord, Tile& rectangle);
	//void FillLightData();

	vector<Text> textbuffer;
	Font* basicFont;

	std::vector<Model*> models;
	std::vector<Model*> debugSpheres;

	//What is actually in the view frustum
	std::vector<ModelMesh*> modelsInFrame;
	//int* tileLights;
	TileData* tileData;
protected:
	Tile* screenTiles;
	TileRenderer* tiles;
	std::vector<GSetting*> GComponents;

	bool debugMode = false;
	void UpdateBasicUniforms();
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

	//Handy debug functions
	void DrawDebugLights();
	void RelinkShaders();

	//Text Stuff
	Shader* textShader;

	void DrawTextBuffer();
	void DrawTextOBJ(const Text& textobj);

	//Other Stuff
	Camera*	camera;
	Frustum frameFrustum;
	//Light*	light;

	Window* wparent;
	float timer = 0;
	float overlayFlags[3];
};
