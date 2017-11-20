#include "Skybox.h"

#include "../Rendering/ShaderTypes/Shader.h"
#include "../../Game/GraphicsConfiguration/GLConfig.h"

float skyboxVertices[] = {
	// positions          
	-1.0f,  1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	1.0f, -1.0f, -1.0f,
	1.0f, -1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f,  1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	-1.0f,  1.0f, -1.0f,
	1.0f,  1.0f, -1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	1.0f, -1.0f,  1.0f
};

Skybox::Skybox(Matrix4* viewMatrix)
{
	skyboxShader = new Shader(SHADERDIR"/Skybox/SkyboxVert.glsl", SHADERDIR"/Skybox/SkyboxFrag.glsl");
	this->viewMatrix = viewMatrix;
}

Skybox::~Skybox()
{
	delete skyboxShader;
}

void Skybox::LinkShaders()
{
	skyboxShader->LinkProgram();
}

void Skybox::RegenerateShaders()
{
	skyboxShader->Regenerate();
}

void Skybox::Initialise()
{
	InitialiseMesh();
}

void Skybox::Apply()
{
	glDepthFunc(GL_LEQUAL);
	glDisable(GL_CULL_FACE);

	glUseProgram(skyboxShader->GetProgram());

	glUniformMatrix4fv(glGetUniformLocation(skyboxShader->GetProgram(),
		"viewMatrix"), 1, false, (float*)viewMatrix);
	glUniformMatrix4fv(glGetUniformLocation(skyboxShader->GetProgram(),
		"projMatrix"), 1, false, (float*)&GLConfig::SHARED_PROJ_MATRIX);

	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(skyboxShader->GetProgram(), "skybox"), 0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);

	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
}

void Skybox::InitialiseMesh()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), 
		&skyboxVertices, GL_STATIC_DRAW);

	//Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);
}

