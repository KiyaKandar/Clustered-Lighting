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

	unsigned int TextureFromFile(const char *path, const string &directory);

	//Modifiers
	void Translate(Vector3 translation) const;
	void Scale(Vector3 scale) const;
	void Rotate(Vector3 axis, float degrees) const;

	//Model Data 
	std::vector<ModelMesh*> meshes;
	std::string directory;
	std::vector<Texture> loadedTextures;

	Assimp::Importer import;
	const aiScene* scene;
};

