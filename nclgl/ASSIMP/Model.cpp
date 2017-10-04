#include "Model.h"

#include <limits>
#include <algorithm>

typedef ModelMesh* (*processmeshFunc)(aiMesh*, aiScene*);

void Model::LoadModel(std::string path)
{
	Assimp::Importer import;
	const aiScene *scene = import.ReadFile(path,
		aiProcess_Triangulate | aiProcess_FlipUVs |
		aiProcess_GenNormals | aiProcess_CalcTangentSpace |
		aiProcess_SplitLargeMeshes | aiProcess_OptimizeMeshes);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
		return;
	}

	directory = path.substr(0, path.find_last_of('/'));

	ProcessNode(scene->mRootNode, scene);

}

void Model::ProcessNode(aiNode *node, const aiScene *scene)
{
	//Process all the node's meshes (if any)
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];

		meshes.push_back(ProcessMesh(mesh, scene));

	}

	//Then do the same for each of its children
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		ProcessNode(node->mChildren[i], scene);
	}
}

ModelMesh* Model::ProcessMesh(aiMesh *mesh, const aiScene *scene)
{
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	vector<Texture> textures;
	vector<Texture> heights;

	const float maxFloat = (std::numeric_limits<float>::max)();
	const float minFloat = (std::numeric_limits<float>::lowest());

	Vector3 minBounds(maxFloat, maxFloat, maxFloat);
	Vector3 maxBounds(minFloat, minFloat, minFloat);

	BoundingBox AABB;

	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;

		//Process vertex positions, normals and texture coordinates
		Vector3 pos;
		pos.x = mesh->mVertices[i].x;
		pos.y = mesh->mVertices[i].y;
		pos.z = mesh->mVertices[i].z;
		vertex.Position = pos;

		//Check all points against both min and max to form AABB
		if (pos.x < minBounds.x) minBounds.x = pos.x;
		if (pos.y < minBounds.y) minBounds.y = pos.y;
		if (pos.z < minBounds.z) minBounds.z = pos.z;

		if (pos.x > maxBounds.x) maxBounds.x = pos.x;
		if (pos.y > maxBounds.y) maxBounds.y = pos.y;
		if (pos.z > maxBounds.z) maxBounds.z = pos.z;

		AABB.min = minBounds;
		AABB.max = maxBounds;

		Vector3 norm;
		norm.x = mesh->mNormals[i].x;
		norm.y = mesh->mNormals[i].y;
		norm.z = mesh->mNormals[i].z;
		vertex.Normal = norm;

		if (mesh->mTextureCoords[0]) //Does the mesh contain texture coordinates?
		{
			Vector2 tex;
			tex.x = mesh->mTextureCoords[0][i].x;
			tex.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = tex;
		}
		else vertex.TexCoords = Vector2(0.0f, 0.0f);

		//Tangent
		Vector3 tan;
		tan.x = mesh->mTangents[i].x;
		tan.y = mesh->mTangents[i].y;
		tan.z = mesh->mTangents[i].z;
		vertex.Tangent = tan;

		//Bitangent
		Vector3 bitan;
		bitan.x = mesh->mBitangents[i].x;
		bitan.y = mesh->mBitangents[i].y;
		bitan.z = mesh->mBitangents[i].z;
		vertex.Bitangent = bitan;

		vertices.push_back(vertex);
	}

	//Process indices
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];

		for (unsigned int j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	//Process material
	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

		vector<Texture> diffuseMaps = LoadMaterialTextures(material,
			aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

		vector<Texture> specularMaps = LoadMaterialTextures(material,
			aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

		vector<Texture> heightMaps = LoadMaterialTextures(material,
			aiTextureType_HEIGHT, "texture_height");
		heights.insert(heights.end(), heightMaps.begin(), heightMaps.end());
	}

	return new ModelMesh(vertices, indices, textures, heights, AABB);
}

vector<Texture> Model::LoadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName)
{
	vector<Texture> textures;

	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		bool skip = false;

		for (unsigned int j = 0; j < loadedTextures.size(); j++)
		{
			if (std::strcmp(loadedTextures[j].path.c_str(), str.C_Str()) == 0)
			{
				textures.push_back(loadedTextures[j]);
				skip = true;
				break;
			}
		}

		//If texture hasn't been loaded already, load it
		if (!skip)
		{
			Texture texture;
			texture.id = TextureFromFile(str.C_Str(), directory);
			texture.type = typeName;
			texture.path = str.C_Str();
			textures.push_back(texture);

			loadedTextures.push_back(texture); //Add to loaded textures
		}
	}

	return textures;
}

unsigned int Model::TextureFromFile(const char *path, const string &directory)
{
	string filename = string(path);
	filename = directory + '/' + filename;

	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

void Model::Translate(Vector3 translation) const
{
	for each (ModelMesh* mesh in meshes)
	{
		mesh->SetPosition(translation);
	}
}

void Model::Scale(Vector3 scale) const
{
	for each (ModelMesh* mesh in meshes)
	{
		mesh->SetScale(scale);
	}
}
