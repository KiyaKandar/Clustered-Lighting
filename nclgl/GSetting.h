#pragma once

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glew32.lib")

#include "GL/glew.h"
#include "GL/wglew.h"
#include "SOIL.h"

#include "Shader.h"
#include "Matrix4.h"
#include "Vector2.h"

//Global vars
const int GPOSITION = 0;
const int GNORMAL	= 1;
const int GALBEDO	= 2;

const int NUM_X_AXIS_TILES = 10;
const int NUM_Y_AXIS_TILES = 10;
const int NUM_Z_AXIS_TILES = 10;

const Vector2 MIN_NDC_COORDS = Vector2(-1, -1);
const Vector2 MAX_NDC_COORDS = Vector2(1, 1);

//DataIndexes
const int SSAO_INDEX	= 0;

struct ShadowData
{
	int NUM_LIGHTS;
	int* shadowIndexes;
	Matrix4* textureMatrices;
	GLuint* shadows;
};

struct GBufferData {
	GLuint* gPosition;
	GLuint* gNormal;
	GLuint* gAlbedo;
};

struct AmbientTextures {
	GLuint** textures;
	int*	 texUnits;
};

class GSetting
{
public:

	GSetting(Vector2 resolution)
	{
		this->resolution = resolution;
		projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)resolution.x / (float)resolution.y, 45.0f);
	}

	virtual ~GSetting() {}

	virtual void LinkShaders()		= 0;
	virtual void Initialise()		= 0;
	virtual void Apply()			= 0;

	virtual void RegenerateShaders() = 0;

	void SetCurrentShader(Shader*s)
	{
		currentShader = s;

		glUseProgram(s->GetProgram());
	}

	void UpdateShaderMatrices()
	{
		if (currentShader)
		{
			glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "modelMatrix"), 1, false, (float*)&modelMatrix);
			glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "viewMatrix"), 1, false, (float*)&viewMatrix);
			glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "projMatrix"), 1, false, (float*)&projMatrix);
			glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "textureMatrix"), 1, false, (float*)&textureMatrix);
			glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "camMatrix"), 1, false, (float*)&viewMatrix);
			currentShader->SetInt("resolutionY", resolution.y);
			currentShader->SetInt("resolutionX", resolution.x);
		}
	}

	//Check framebuffer is complete
	void CheckBuffer(std::string name) {
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			std::cout << name + "-buffer not complete!" << std::endl;
		}
	}

protected:
	virtual void LocateUniforms() = 0;

	void RenderScreenQuad()
	{
		if (quadVAO == 0)
		{
			float quadVertices[] = {
				// positions        // texture Coords
				-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
				-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
				1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
				1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
			};

			// setup plane VAO
			glGenVertexArrays(1, &quadVAO);
			glGenBuffers(1, &quadVBO);
			glBindVertexArray(quadVAO);
			glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		}

		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);
	}

	unsigned int quadVAO = 0;
	unsigned int quadVBO;

	Vector2 resolution;

	Shader* currentShader;

	Matrix4 projMatrix;		//Projection matrix
	Matrix4 modelMatrix;	//Model matrix. NOT MODELVIEW
	Matrix4 viewMatrix;		//View matrix
	Matrix4 textureMatrix;	//Texture matrix
};

