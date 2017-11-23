#pragma once

#include "../nclgl/Maths/Vector3.h"
#include "../../nclgl/GraphicsSettings/Settings/Shadows.h"

#include <vector>
#include <string>
#include <unordered_map>
#include <memory>
#include <algorithm>
#include <functional>
#include "../../nclgl/Assets/ParticleSystem.h"

class Model;
class Light;
class Renderer;

class Scene
{
public:
	Scene(std::vector<std::string> skyBoxTextures, std::vector<std::string> reflectionTextures, 
		std::vector<std::pair<std::string, int>> modelsToLoad, Vector3 lightWorkGroups, float ambient);
	~Scene();

	void LoadModels();
	void InitialiseShadows(int numShadowCastingLights, Renderer* renderer);
	void UpdateScene(int frameCount);

	void AddUpdateProcess(std::function<void(float)> update);
	void AddLight(Light* light, int defaultIndexToReplace);
	Model* GetModel(const std::string& fileName);

	std::vector<Model*>* GetModels();
	const std::vector<std::pair<Light*, int>>& GetSceneLights() const;

	unsigned int GetSkyboxTextureID()
	{
		return skyBoxTextureID;
	}

	unsigned int GetReflectionCubeMapTextureID()
	{
		return reflectionTextureID;
	}

	ShadowData* GetShadowData()
	{
		return shadows->GetShadowData();
	}

	void SetLightColour(int index, Vector4 colour)
	{
		sceneLights[index].first->SetColour(colour);
		modifiedLights.push_back(index);
	}

	void SetLightDirection(int index, Vector4 direction)
	{
		sceneLights[index].first->SetDirection(direction);
		modifiedLights.push_back(index);
	}

	void SetLightPosition(int index, Vector3 position)
	{
		sceneLights[index].first->SetPosition(position);
		modifiedLights.push_back(index);
	}

	Vector3* lightWorkGroups;
	vector<Particle> particles;
	vector<int> modifiedLights;
	float ambient;

private:
	void AddModel(const std::string& fileName, int numDuplicates);
	void LoadCubemap(unsigned int* textureID, std::vector<std::string> textures);

	std::vector<Model*>* modelsVec;
	std::unordered_map<std::string, Model*> models;
	std::vector<std::pair<std::string, int>> modelsToLoad;
	std::vector<std::pair<Light*, int>> sceneLights;
	std::vector<std::function<void(float)>> updates;
	std::vector<std::string> skyBoxTextures;
	std::vector<std::string> reflectionTextures;

	int modelIDCount;
	unsigned int skyBoxTextureID;
	unsigned int reflectionTextureID;
	Shadows* shadows;
};

