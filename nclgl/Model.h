#pragma once
#pragma comment(lib, "assimp-vc140-mt.lib")

#include "ModelMesh.h"

#include <Importer.hpp>
#include <scene.h>
#include <postprocess.h>

#include <iostream>
#include <string>
#include <vector>

#include <Simple OpenGL Image Library\src\stb_image_aug.h>

class Model
{
public:

	Model(char *path)
	{
		LoadModel(path);
	}

	~Model() {};

	void LoadModel(std::string path);

	void ProcessNode(aiNode *node, const aiScene *scene);
	ModelMesh* ProcessMesh(aiMesh *mesh, const aiScene *scene);

	std::vector<Texture> LoadMaterialTextures(aiMaterial *mat, aiTextureType type,
		string typeName);

	unsigned int TextureFromFile(const char *path, const string &directory);

	//Modifiers
	void Translate(Vector3 translation);
	void Scale(Vector3 scale);

	//Model Data 
	std::vector<ModelMesh*> meshes;
	std::string directory;
	std::vector<Texture> loadedTextures;
};

