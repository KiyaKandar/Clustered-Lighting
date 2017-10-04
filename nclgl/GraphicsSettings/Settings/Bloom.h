#pragma once

#include "../SettingsType/GSetting.h"

class Bloom : public GSetting
{
public:
	explicit Bloom(int strength);
	~Bloom();

	void LinkShaders() override;
	void RegenerateShaders() override;

	void Initialise() override;
	void Apply() override;

	GLuint FBO;
private:
	void CreateTexture();
	void InitialiseBlur();

	void LocateUniforms() override;

	void ApplyBlur();
	void Combine();

	//Shader* bloomShader;
	Shader* blurShader;
	Shader* combineShader;

	//Brightness tex
	GLuint colourBuffers[2];
	GLuint attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };

	//Uniform locations
	GLint loc_horizontal;
	GLint loc_image;
	GLint loc_scene;
	GLint loc_bloomBlur;

	//Blur tex
	GLuint pingpongFBO[2];
	GLuint pingpongColourBuffers[2];
	GLuint rboDepth;

	bool horizontal;
	bool first_iteration;

	int blurStrength;
};

