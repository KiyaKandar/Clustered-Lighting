#pragma once

#include "../GraphicsSettings/SettingsType/GSetting.h"

struct Particle
{
	Matrix4 modelMatrix;
	Vector3 originalPosition;
	Vector3 frameTranslation;
	float particleSize;
	float alphaDecay;
	float decayRate;

	Particle(Vector3 startPosition, Vector3 translation,
		float size, float rate)
	{
		originalPosition = startPosition;
		modelMatrix = Matrix4::Translation(originalPosition);
		frameTranslation = translation;
		alphaDecay = 0.0f;
		particleSize = size;
		decayRate = rate;
	}
};

class ParticleSystem : public GSetting
{
public:
	explicit ParticleSystem(Matrix4* viewMatrix);
	~ParticleSystem();

	void LinkShaders() override;
	void RegenerateShaders() override;

	void Initialise() override;
	void Apply() override;

	GLuint* motionBlurFBO;

	vector<Particle>* particles;
private:
	void InitialiseMesh();
	GLuint LoadTexture(string name);
	void LocateUniforms() override {}

	Matrix4* viewMatrix;
	//Matrix4 modelMatrix;

	Shader* particleShader;

	unsigned int VAO;
	unsigned int VBO;
	unsigned int TVBO;
	GLuint textureID;
	GLuint textureID1;

	float frameCount = 0;
};

