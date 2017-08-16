#pragma once

#include "GSetting.h"
#include "Camera.h"

#include <vector>

class BPLighting : public GSetting
{
public:
	BPLighting(Vector2 resolution, Camera* cam, 
		GBufferData* gBuffer, ShadowData* shadowData,
		AmbientTextures* ambientTextures, int numAmbTex);

	virtual ~BPLighting() {
		delete lightingPass;
	}

	void LinkShaders();
	void RegenerateShaders();

	void Initialise();
	void Apply();

	GLuint* FBO;
private:
	void LocateUniforms();
	void LightingPass();

	//Uniform locations
	GLint loc_gPosition;
	GLint loc_gNormal;
	GLint loc_gAlbedo;
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

