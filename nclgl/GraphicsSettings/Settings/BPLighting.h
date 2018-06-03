#pragma once

#include "../SettingsType/GSetting.h"
#include "../../Rendering/View/Camera.h"

#include <vector>

class BPLighting : public GSetting
{
public:
	BPLighting(Camera* cam, GBufferData* gBuffer,
		AmbientTextures* ambientTextures, int numAmbTex);

	virtual ~BPLighting()
	{
		delete lightingPass;
	}

	void LinkShaders()  override;
	void RegenerateShaders()  override;

	void UpdateShadowData(ShadowData* shadowData) 
	{
		this->shadowData = shadowData;
	}

	void Initialise() override;
	void Apply() override;

	GLuint* FBO;
	float ambientLighting;

private:
	void LocateUniforms() override;
	void LightingPass();

	//Uniform locations
	GLint loc_gPosition;
	GLint loc_gNormal;
	GLint loc_gAlbedo;
	GLint loc_gMetallic;
	GLint loc_shadows;
	GLint loc_ambientTextures;
	GLint loc_texMatrices;
	GLint loc_numXTiles;
	GLint loc_numYTiles;
	GLint loc_numberOfLights;
	GLint loc_camMatrix;
	GLint loc_numShadowCastingLights;

	Shader*		lightingPass;
	Camera*		camera;
	ShadowData* shadowData;
	GBufferData*	gBuffer;
	AmbientTextures* ambientTextures;
	int numAmbTex;
};

