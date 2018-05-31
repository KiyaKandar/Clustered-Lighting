#pragma once

#include "../SettingsType/GSetting.h"
#include "../../ASSIMP/ModelMesh.h"
#include "../../Rendering/View/Camera.h"

class DepthPass : public GSetting
{
public:
	DepthPass(Camera* camera, std::vector<ModelMesh*>* modelsInFrame);
	~DepthPass();

	void LinkShaders() override;
	void RegenerateShaders() override;

	void Initialise() override;
	void Apply() override;

	GLuint depthFrameBuffer;
	GLuint depthTexture;

private:
	void RenderGeometry();
	void LocateUniforms() override;

	std::vector<ModelMesh*>* modelsInFrame;

	Shader* depthPassShader;

	Camera* camera;
	Window* window;
};

