#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <memory>
#include <algorithm>

class Model;

class Scene
{
public:
	Scene(std::vector<std::pair<std::string, int>> modelsToLoad);
	~Scene();

	void LoadModels();

	std::vector<Model*>* GetModels()
	{
		return &models;
	}

	Model* GetModel(const std::string& fileName, int duplicateNum = 0);

private:
	void AddModel(const std::string& fileName);

	std::vector<Model*> models;
	std::vector<std::pair<std::string, int>> modelsToLoad;
	int modelIDCount;
};

