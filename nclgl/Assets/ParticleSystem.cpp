#include "ParticleSystem.h"

#include "../ASSIMP/Model.h"

float vertices[] =
{
	0.0f, 0.0f, 0.0f
};

ParticleSystem::ParticleSystem(Matrix4* viewMatrix)
{
	particleShader = new Shader(SHADERDIR"/Particles/pointVert.glsl",
		SHADERDIR"/Particles/pointFrag.glsl",
		SHADERDIR"/Particles/pointGeo.glsl", true);
	this->viewMatrix = viewMatrix;
}

ParticleSystem::~ParticleSystem()
{
	delete particleShader;
}

void ParticleSystem::LinkShaders()
{
	particleShader->LinkProgram();
}

void ParticleSystem::RegenerateShaders()
{
	particleShader->Regenerate();
}

void ParticleSystem::Initialise()
{
	InitialiseMesh();
}

void ParticleSystem::Apply()
{
	glBindFramebuffer(GL_FRAMEBUFFER, *motionBlurFBO);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glUseProgram(particleShader->GetProgram());

	glUniformMatrix4fv(glGetUniformLocation(particleShader->GetProgram(),
		"viewMatrix"), 1, false, (float*)viewMatrix);
	glUniformMatrix4fv(glGetUniformLocation(particleShader->GetProgram(),
		"projMatrix"), 1, false, (float*)&GLConfig::SHARED_PROJ_MATRIX);

	for (int i = 0; i < particles->size(); ++i)
	{
		(*particles)[i].modelMatrix = (*particles)[i].modelMatrix * Matrix4::Translation((*particles)[i].frameTranslation);

		glUniformMatrix4fv(glGetUniformLocation(particleShader->GetProgram(),
			"modelMatrix"), 1, false, (float*)&(*particles)[i].modelMatrix);

		glUniform1f(glGetUniformLocation(particleShader->GetProgram(), "particleSize"), (*particles)[i].particleSize);
		glUniform1f(glGetUniformLocation(particleShader->GetProgram(), "alphaDecay"), (*particles)[i].alphaDecay);
		glUniform4fv(glGetUniformLocation(particleShader->GetProgram(), "colour"), 1, (float*)&(*particles)[i].colour);

		glBindVertexArray(VAO);
		glDrawArrays(GL_POINTS, 0, 1);
		glBindVertexArray(0);

		(*particles)[i].alphaDecay += (*particles)[i].decayRate;

		if ((*particles)[i].alphaDecay >= 1.0f)
		{
			(*particles)[i].alphaDecay = 0.0f;
			(*particles)[i].modelMatrix = Matrix4::Translation((*particles)[i].originalPosition);
		}
	}

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	++frameCount;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ParticleSystem::InitialiseMesh()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), 
		&vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
}