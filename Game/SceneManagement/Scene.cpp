#include "Scene.h"

#include "../../nclgl/ASSIMP/Model.h"

Scene::Scene(vector<pair<string, int>> modelsToLoad, Vector3 lightWorkGroups)
{
	modelIDCount = 0;
	this->modelsToLoad = modelsToLoad;
	this->lightWorkGroups = lightWorkGroups;
}

Scene::~Scene()
{
	for each (Model* model in models)
	{
		delete model;
	}
}

void Scene::LoadModels()
{
	for each (pair<string, int> file in modelsToLoad)
	{
		for (int i = 0; i < file.second; ++i)
		{
			AddModel(file.first);
		}
	}
}

void Scene::AddLight(Light* light, int defaultIndexToReplace)
{
	sceneLights.push_back(make_pair(light, defaultIndexToReplace));
}

Model* Scene::GetModel(const string& fileName, int duplicateNum)
{
	int count = 0;

	vector<pair<string, int>>::iterator pairIterator = find_if(modelsToLoad.begin(), 
		modelsToLoad.end(), [fileName, &count](const pair<string, int>& p)
	{
		if(p.first == fileName)
		{
			return true;
		}
		else
		{
			count += p.second;
			return false;
		}
	});

	return models[count + duplicateNum];
}

void Scene::AddModel(const string& fileName)
{
	models.push_back(new Model(fileName));
	modelIDCount++;
}

vector<Model*>* Scene::GetModels()
{
	return &models;
}

const std::vector<std::pair<Light*, int>>& Scene::GetSceneLights() const
{
	return sceneLights;
}

const Vector3 Scene::GetLightWorkGroups() const
{
	return lightWorkGroups;
}
