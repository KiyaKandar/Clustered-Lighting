#pragma once

#include "../nclgl/Maths/Vector3.h"

#include <vector>
#include <string>
#include <unordered_map>
#include <memory>
#include <algorithm>

class Model;
class Light;

class Scene
{
public:
	Scene(std::vector<std::pair<std::string, int>> modelsToLoad, Vector3 lightWorkGroups);
	~Scene();

	void LoadModels();

	void AddLight(Light* light, int defaultIndexToReplace);
	Model* GetModel(const std::string& fileName, int duplicateNum = 0);

	std::vector<Model*>* GetModels();
	const std::vector<std::pair<Light*, int>>& GetSceneLights() const;
	const Vector3 GetLightWorkGroups() const;

private:
	void AddModel(const std::string& fileName);

	std::vector<Model*> models;
	std::vector<std::pair<std::string, int>> modelsToLoad;
	std::vector<std::pair<Light*, int>> sceneLights;

	Vector3 lightWorkGroups;
	int modelIDCount;
};

