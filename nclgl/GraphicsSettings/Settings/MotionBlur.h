#pragma once

#include "../SettingsType/GSetting.h"

class MotionBlur : public GSetting
{
public:
	MotionBlur(GBufferData* gBuffer,  
		Matrix4* previousView, Matrix4* currentView, float* fps);
	~MotionBlur();

	void LinkShaders() override;
	void RegenerateShaders() override;

	void Initialise() override;
	void Apply() override;

	GLuint* FBO;

private:
	void CreateTexture();
	void LocateUniforms() override {}

	Shader* blurShader;

	GLuint colourBuffer[1];
	GLuint attachment[1] = { GL_COLOR_ATTACHMENT2 };
	GLuint rboDepth;

	GBufferData* gBuffer;

	Matrix4* previousView;
	Matrix4* currentView;
	float* fps;
};

