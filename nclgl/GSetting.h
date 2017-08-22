#pragma once

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glew32.lib")

#include "GL/glew.h"
#include "GL/wglew.h"
#include "SOIL.h"

#include "Shader.h"
#include "Matrix4.h"
#include "Vector2.h"

#include "../Game/GLConfig.h"

#include <unordered_map>

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

	GSetting()
	{
		projMatrix = GLConfig::SHARED_PROJ_MATRIX;//Matrix4::Perspective(nearPlane, farPlane, (float)resolution.x / (float)resolution.y, fov);
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
			currentShader->SetInt("resolutionY", GLConfig::RESOLUTION.y);
			currentShader->SetInt("resolutionX", GLConfig::RESOLUTION.x);
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

	Shader* currentShader;

	Matrix4 projMatrix;		
	Matrix4 modelMatrix;	
	Matrix4 viewMatrix;		
	Matrix4 textureMatrix;	
};

