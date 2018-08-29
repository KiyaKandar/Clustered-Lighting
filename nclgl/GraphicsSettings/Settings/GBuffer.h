#pragma once

#include "../SettingsType/GSetting.h"

#include "../../Rendering/View/Camera.h"
#include "../../ASSIMP/ModelMesh.h"
#include "../../ASSIMP/Model.h"

#include <vector>
#include "../../Assets/Skybox.h"
#include "../../Assets/ParticleSystem.h"

class GBuffer :
	public GSetting
{
public:
	GBuffer(Window* window, Camera* camera, std::vector<ModelMesh*>* modelsInFrame,
		vector<ModelMesh*>* transparentModelsInFrame, std::vector<Model*>** models);
	virtual ~GBuffer();

	void LinkShaders() override;
	void RegenerateShaders() override;

	void Initialise() override;
	void Apply() override;

	GBufferData* GetGBuffer() const
	{
		return SGBuffer;
	}

	void SetReflectionTextureID(unsigned int newTextureID)
	{
		textureID = newTextureID;
	}

	GLuint rboDepth;
	GLuint gBuffer;

private:
	void LocateUniforms() override;
	void InitGBuffer();
	void InitAttachments();

	void RenderGeometry(vector<ModelMesh*>* meshes);

	GLuint gPosition;
	GLuint gNormal;
	GLuint gAlbedo;
	GLuint gMetallic;
	GLuint gRoughness;
	GLuint attachments[5] =
	{ GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, 
		GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4 };

	Shader* geometryPass;

	Camera* camera;
	Window* window;

	std::vector<ModelMesh*>* modelsInFrame;
	std::vector<ModelMesh*>* transparentModelsInFrame;
	std::vector<Model*>** models;
	GBufferData* SGBuffer;

	unsigned int textureID;

	GLint loc_skybox;
	GLint loc_cameraPos;
	GLint loc_hasTexture;
	GLint loc_isReflective;
	GLint loc_reflectionStrength;
	GLint loc_baseColour;
};

