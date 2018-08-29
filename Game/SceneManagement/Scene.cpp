#include "Scene.h"

#include "../../nclgl/ASSIMP/Model.h"
#include "../../nclgl/Rendering/Renderer/Renderer.h"

Scene::Scene(std::vector<std::string> skyBoxTextures, std::vector<std::string> reflectionTextures, 
	vector<pair<string, int>> modelsToLoad, Vector3 lightWorkGroups, float ambient)
{
	modelIDCount = 0;

	modelsVec = new vector<Model*>();
	this->lightWorkGroups = new Vector3();

	this->modelsToLoad = modelsToLoad;
	*this->lightWorkGroups = lightWorkGroups;
	this->skyBoxTextures = skyBoxTextures;
	this->reflectionTextures = reflectionTextures;
	this->ambient = ambient;

	LoadCubemap(&skyBoxTextureID, skyBoxTextures);
	LoadCubemap(&reflectionTextureID, reflectionTextures);
}

Scene::~Scene()
{
	for each (Model* model in *modelsVec)
	{
		delete model;
	}

	delete modelsVec;
	delete lightWorkGroups;

	glDeleteTextures(1, &skyBoxTextureID);
	glDeleteTextures(1, &reflectionTextureID);
}

void Scene::LoadModels()
{
	for each (pair<string, int> file in modelsToLoad)
	{
		AddModel(file.first, file.second);
	}
}

void Scene::UpdateScene(int frameCount)
{
	for each (function<void(float)> update in updates)
	{
		update(frameCount);
	}
}

void Scene::AddUpdateProcess(std::function<void(float)> update)
{
	updates.push_back(update);
}

void Scene::AddLight(Light* light, int defaultIndexToReplace)
{
	sceneLights.push_back(make_pair(light, defaultIndexToReplace));
}

Model* Scene::GetModel(const string& fileName)
{
	return models[fileName];
}

void Scene::AddModel(const string& fileName, int numDuplicates)
{
	Model* model = new Model(fileName, numDuplicates);
	modelsVec->push_back(model);
	models.insert({ fileName, model });
	modelIDCount++;
}

vector<Model*>* Scene::GetModels()
{
	return modelsVec;
}

const std::vector<std::pair<Light*, int>>& Scene::GetSceneLights() const
{
	return sceneLights;
}

void Scene::LoadCubemap(unsigned int* textureID, std::vector<std::string> textures)
{
	glGenTextures(1, textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, *textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < textures.size(); i++)
	{
		unsigned char *data = stbi_load(textures[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
			);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << textures[i] << std::endl;
			stbi_image_free(data);
		}
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}
