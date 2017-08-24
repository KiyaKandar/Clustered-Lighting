#include "BPLighting.h"

#include "../Game/GLConfig.h"

BPLighting::BPLighting(Camera* cam, GBufferData* gBuffer, ShadowData* shadowData,
	AmbientTextures* ambientTextures, int numAmbTex)
{
	camera = cam;

	this->shadowData		= shadowData;
	this->gBuffer			= gBuffer;
	this->ambientTextures	= ambientTextures;
	this->numAmbTex			= numAmbTex; 
	
	lightingPass = new Shader(SHADERDIR"/SSAO/ssao_lightingvert.glsl", SHADERDIR"/SSAO/ssao_lightingfrag.glsl", "", false);
}

void BPLighting::LinkShaders()
{
	lightingPass->LinkProgram();
}

void BPLighting::RegenerateShaders()
{
	lightingPass->Regenerate();
}

void BPLighting::Initialise()
{
	LocateUniforms();
}

void BPLighting::LocateUniforms()
{
	loc_gPosition		= glGetUniformLocation(lightingPass->GetProgram(), "gPosition");
	loc_gNormal			= glGetUniformLocation(lightingPass->GetProgram(), "gNormal");
	loc_gAlbedo			= glGetUniformLocation(lightingPass->GetProgram(), "gAlbedo");
	loc_shadows			= glGetUniformLocation(lightingPass->GetProgram(), "shadows");
	loc_ambientTextures = glGetUniformLocation(lightingPass->GetProgram(), "ambientTextures");
	loc_texMatrices		= glGetUniformLocation(lightingPass->GetProgram(), "texMatrices");
	loc_numXTiles		= glGetUniformLocation(lightingPass->GetProgram(), "numXTiles");
	loc_numYTiles		= glGetUniformLocation(lightingPass->GetProgram(), "numYTiles");
	loc_numberOfLights	= glGetUniformLocation(lightingPass->GetProgram(), "numberOfLights");
	loc_camMatrix		= glGetUniformLocation(lightingPass->GetProgram(), "camMatrix");

	loc_numShadowCastingLights = 
		glGetUniformLocation(lightingPass->GetProgram(), "numShadowCastingLights");
}

void BPLighting::Apply() {
	LightingPass();
}

void BPLighting::LightingPass()
{
	glBindFramebuffer(GL_FRAMEBUFFER, *FBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	SetCurrentShader(lightingPass);

	glUniform1i(loc_gPosition,	GLConfig::GPOSITION);
	glUniform1i(loc_gNormal,	GLConfig::GNORMAL);
	glUniform1i(loc_gAlbedo,	GLConfig::GALBEDO);

	glUniform1i(loc_numXTiles, GLConfig::NUM_X_AXIS_TILES);
	glUniform1i(loc_numYTiles, GLConfig::NUM_Y_AXIS_TILES);
	glUniform1i(loc_numberOfLights, shadowData->NUM_LIGHTS);
	glUniform1i(loc_numShadowCastingLights, GLConfig::SHADOW_LIGHTS);

	glUniform1iv(loc_shadows, shadowData->NUM_LIGHTS, shadowData->shadowIndexes);
	glUniform1iv(loc_ambientTextures, numAmbTex, ambientTextures->texUnits);

	glUniformMatrix4fv(loc_texMatrices, shadowData->NUM_LIGHTS, false, (float*)&*shadowData->textureMatrices);

	viewMatrix = camera->BuildViewMatrix();
	glUniformMatrix4fv(loc_camMatrix, 1, false, (float*)&viewMatrix);

	UpdateShaderMatrices();

	currentShader->ApplyTexture(GLConfig::GPOSITION,	*gBuffer->gPosition);
	currentShader->ApplyTexture(GLConfig::GNORMAL,		*gBuffer->gNormal);
	currentShader->ApplyTexture(GLConfig::GALBEDO,		*gBuffer->gAlbedo);

	for (int a = 0; a < numAmbTex; ++a)
	{
		glActiveTexture(GL_TEXTURE3 + a);
		glBindTexture(GL_TEXTURE_2D, *ambientTextures->textures[a]);
	}

	for (int s = 0; s < shadowData->NUM_LIGHTS; ++s)
	{
		glActiveTexture(GL_TEXTURE4 + s + (numAmbTex - 1));
		glBindTexture(GL_TEXTURE_2D, shadowData->shadows[s]);
	}

	RenderScreenQuad();
}