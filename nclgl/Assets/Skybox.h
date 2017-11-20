#pragma once

#include "../GraphicsSettings/SettingsType/GSetting.h"
#include "../Maths/Matrix4.h"

#include <vector>
#include <string>
#include <Simple OpenGL Image Library\src\stb_image_aug.h>

class Skybox : public GSetting
{
public:
	Skybox(Matrix4* viewMatrix);
	~Skybox();

	void LinkShaders() override;
	void RegenerateShaders() override;

	void Initialise() override;
	void Apply() override;

	void SetSkyboxTexture(unsigned int newTextureID)
	{
		textureID = newTextureID;
	}

	GLuint* GBufferFBO;

private:
	void InitialiseMesh();
	void LocateUniforms() override {}

	Matrix4* viewMatrix;
	Shader* skyboxShader;
	unsigned int textureID;
	unsigned int VAO, VBO;
};

