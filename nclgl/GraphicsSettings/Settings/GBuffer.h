#pragma once

#include "../SettingsType/GSetting.h"

#include "../../Rendering/View/Camera.h"
#include "../../ASSIMP/ModelMesh.h"
#include "../../ASSIMP/Model.h"

#include <vector>

class GBuffer :
	public GSetting
{
public:
	GBuffer(Window* window, Camera* camera, std::vector<ModelMesh*>* modelsInFrame,
		std::vector<Model*>** models);
	virtual ~GBuffer();

	void LinkShaders() override;
	void RegenerateShaders() override;

	void Initialise() override;
	void Apply() override;

	GBufferData* GetGBuffer() const
	{
		return SGBuffer;
	}

private:
	void LocateUniforms() override
	{}
	void InitGBuffer();
	void InitAttachments();

	void FillGBuffer();

	GLuint gBuffer;
	GLuint gPosition;
	GLuint gNormal;
	GLuint gAlbedo;
	GLuint attachments[3] =
	{ GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	GLuint rboDepth;

	Shader* geometryPass;

	Camera* camera;
	Window* window;

	std::vector<ModelMesh*>* modelsInFrame;
	std::vector<Model*>** models;
	GBufferData* SGBuffer;
};

