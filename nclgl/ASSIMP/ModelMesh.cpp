#include "ModelMesh.h"

ModelMesh::ModelMesh(vector<Vertex> vertices, vector<unsigned int> indices,
	vector<Texture> textures, vector<Texture> heights,
	BoundingBox AABB, int numTransforms)
{
	this->vertices = vertices;
	this->indices = indices;
	this->textures = textures;
	this->heights = heights;

	box = AABB;
	boundingRadius = 1.0f;
	distanceFromCamera = 0.0f;
	baseColour = Vector4(0.6f, 0.6f, 0.6f, 1.0f);

	SetupMesh();

	transforms = vector<Matrix4>(numTransforms);

	for each (Matrix4 transform in transforms)
	{
		transform.SetPositionVector(Vector3(0, 0, 0));
		transform.SetScalingVector(Vector3(1, 1, 1));
	}

	CalculateBoundingRadius();
}

ModelMesh::~ModelMesh()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

void ModelMesh::SetupMesh()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices.at(0), GL_STATIC_DRAW);

	//Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

	//Vertex normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

	//Vertex texture coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

	//Vertex tangents
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));

	//Vertex bitangents
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
		&indices[0], GL_STATIC_DRAW);


	glBindVertexArray(0);
}

void ModelMesh::Draw(Shader& shader)
{
	//Bind all textures of the mesh
	for (unsigned int i = 1; i <= textures.size(); i++)
	{
		//Activate proper texture unit before binding
		glActiveTexture(GL_TEXTURE0 + i);
		glUniform1i(glGetUniformLocation(shader.GetProgram(), textures[i - 1].type.c_str()), i);

		if (textures[i - 1].type == "texture_height")
		{
			glUniform1i(glGetUniformLocation(shader.GetProgram(), "hasBumpMap"), 1);
		}
		else
		{
			glUniform1i(glGetUniformLocation(shader.GetProgram(), "hasBumpMap"), 0);
		}

		glBindTexture(GL_TEXTURE_2D, textures[i - 1].id);
	}

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, modelMatricesSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Matrix4) * transforms.size(),
		(float*)&transforms[0], GL_STATIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 8, modelMatricesSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	glBindVertexArray(VAO);
	glDrawElementsInstanced(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0, transforms.size());
	glBindVertexArray(0);

	glActiveTexture(GL_TEXTURE0);
}

void ModelMesh::DrawShadow(Shader& shader)
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, modelMatricesSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Matrix4) * transforms.size(),
		(float*)&transforms[0], GL_STATIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 8, modelMatricesSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	glBindVertexArray(VAO);
	glDrawElementsInstanced(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0, transforms.size());
	glBindVertexArray(0);
}

void ModelMesh::BufferData()
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, modelMatricesSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Matrix4) * transforms.size(),
		(float*)&transforms[0], GL_STATIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 8, modelMatricesSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void ModelMesh::DrawOnly()
{
	glBindVertexArray(VAO);
	glDrawElementsInstanced(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0, transforms.size());
	glBindVertexArray(0);
}
