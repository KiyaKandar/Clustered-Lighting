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
		BoundingBox AABB, int numTransforms);
	~ModelMesh()
	{}

	void Draw(Shader& shader);
	void DrawShadow(Shader& shader); //Dont bother binding textures.

	float previousradius = 0;
	void SetScale(const Vector3& scale, int matrixNum)
	{
		transforms[matrixNum].SetScalingVector(scale);
		previousradius = boundingRadius;
		box.max = box.max * (scale);
		box.min = box.min * (scale);
		CalculateBoundingRadius();
		//box.min = box.min * scale;
	}

	void SetPosition(const Vector3& position, int matrixNum)
	{
		transforms[matrixNum].SetPositionVector(position);
	}

	void Rotate(const Vector3& axis, const float& degrees, int matrixNum)
	{
		transforms[matrixNum] = transforms[matrixNum] * Matrix4::Rotation(degrees, axis);
	}

	Matrix4* GetTransform(int matrixNum)
	{
		return &transforms[matrixNum];
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
		//float firstMax = max(box.max.x, box.max.y);
		//float finalMax = max(firstMax, box.max.z);
		//boundingRadius = finalMax;
		
		float minLength = abs(box.min.Length());
		float maxLength = abs(box.max.Length());

		boundingRadius = max(minLength, maxLength);//(box.max - box.min).Length() / 2;//
	}

	void BufferData();
	void DrawOnly();

	//Mesh Data
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;
	std::vector<Texture> heights;

	BoundingBox box;

	int hasTexture = 0;
	int isReflective = 0;
	float reflectionStrength = 1.0f;
	Vector4 baseColour;

	GLuint modelMatricesSSBO;

private:
	void SetupMesh();

	unsigned int VAO, VBO, EBO; //Render data

	vector<Matrix4> transforms;

	float boundingRadius;
	float distanceFromCamera;
};

