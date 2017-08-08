#pragma once

#include "GSetting.h"

const int IMAGE = 0;
const int SCENE = 0;
const int BLOOM_BLUR = 1;

class Bloom : public GSetting
{
public:
	Bloom(Vector2 resolution);
	~Bloom();

	void LinkShaders();
	void RegenerateShaders();

	void Initialise();
	void Apply();

	GLuint FBO;
private:
	void LocateUniforms();

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
};

