#pragma once

#include "Vector3.h"
#include "../Rendering/ShaderTypes/Shader.h"
#include "../Maths/Matrix4.h"

#include <sstream>
#include <string>
#include <vector>
#include <matrix4x4.h>

const int NUM_BONES_PER_VEREX = 10;

struct Vertex
{
	Vector3 Position;
	Vector3 Normal;
	Vector2 TexCoords;
	Vector3 Tangent;
	Vector3 Bitangent;
};

struct Texture
{
	unsigned int id;
	std::string type;
	std::string path;  // we store the path of the texture to compare with other textures
};

struct BoundingBox
{
	Vector3 max;
	Vector3 min;
};

class ModelMesh
{
public:
	ModelMesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices,
		std::vector<Texture> textures, std::vector<Texture> heights, 
		BoundingBox AABB);
	~ModelMesh()
	{}

	void Draw(Shader& shader);
	void DrawShadow(Shader& shader); //Dont bother binding textures.

	void SetScale(const Vector3& scale)
	{
		transform.SetScalingVector(scale);

		box.max = box.max * scale;
		CalculateBoundingRadius();
		//box.min = box.min * scale;
	}

	void SetPosition(const Vector3& position)
	{
		transform.SetPositionVector(position);
	}

	void Rotate(const Vector3& axis, const float& degrees)
	{
		transform = transform * Matrix4::Rotation(degrees, axis);
	}

	Matrix4* GetTransform()
	{
		return &transform;
	}

	float GetBoundingRadius() const
	{
		return boundingRadius;
	}

	void SetBoundingRadius(float radius)
	{
		boundingRadius = radius;
	}

	void SetCameraDistance(float dist)
	{
		distanceFromCamera = dist;
	}

	static bool CompareByCameraDistance(const ModelMesh* a, const ModelMesh* b)
	{
		return (a->distanceFromCamera < b->distanceFromCamera)
			? true : false;
	}

	void CalculateBoundingRadius()
	{
		boundingRadius = (box.max - box.min).Length() / 2;
	}

	//Mesh Data
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;
	std::vector<Texture> heights;

	BoundingBox box;

	bool hasTexture;
private:
	void SetupMesh();

	unsigned int VAO, VBO, EBO; //Render data

	Matrix4 transform;

	float boundingRadius;
	float distanceFromCamera;
};

