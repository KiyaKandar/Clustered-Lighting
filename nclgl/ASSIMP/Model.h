#pragma once
#pragma comment(lib, "assimp-vc140-mt.lib")

#include "ModelMesh.h"

#include <Importer.hpp>
#include <scene.h>
#include <postprocess.h>

#include <iostream>
#include <string>
#include <vector>

#include <Simple OpenGL Image Library/src/SOIL.h>

class Model
{
public:

	Model(char *path, int numModels)
	{
		this->numModels = numModels;
		LoadModel(path);
	}

	Model(const string path, int numModels)
	{
		this->numModels = numModels;
		LoadModel(path);
	}

	~Model()
	{
		for each (ModelMesh* mesh in meshes)
		{
			delete mesh;
		}
	};

	void LoadModel(std::string path);

	void ProcessNode(aiNode *node, const aiScene *scene);
	ModelMesh* ProcessMesh(aiMesh *mesh, const aiScene *scene);

	std::vector<Texture> LoadMaterialTextures(aiMaterial *mat, aiTextureType type,
		string typeName);

	static unsigned int TextureFromFile(const char *path, const string &directory);

	//Modifiers
	void Translate(Vector3 translation, int matrixNum = 0) const;
	void Scale(Vector3 scale, int matrixNum = 0) const;
	void Rotate(Vector3 axis, float degrees, int matrixNum = 0) const;

	Vector3 GetPosition(int matrixNum)
	{
		return meshes[0]->GetTransform(matrixNum)->GetPositionVector();
	}

	void SetReflectionAttributesForAllSubMeshes(int isReflective, float strength);
	void SetbackupColourAttributeForAllSubMeshes(Vector4 colour);

	//Model Data 
	std::vector<ModelMesh*> meshes;
	unordered_map<string, ModelMesh*> meshesByName;
	std::string directory;
	std::vector<Texture> loadedTextures;

	Assimp::Importer import;
	const aiScene* scene;

	int numModels;
};

