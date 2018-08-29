#include "BPLighting.h"

#include "../Game/GraphicsConfiguration/GLConfig.h"

BPLighting::BPLighting(Camera* cam, GBufferData* gBuffer,
	AmbientTextures* ambientTextures, int numAmbTex, Window* window)
{
	camera = cam;
	this->window = window;

	this->gBuffer = gBuffer;
	this->ambientTextures = ambientTextures;
	this->numAmbTex = numAmbTex;

	lightingPass = new Shader(SHADERDIR"/SSAO/ssao_lightingvert.glsl", SHADERDIR"/SSAO/ssao_lightingfrag.glsl");
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
	loc_gPosition = glGetUniformLocation(lightingPass->GetProgram(), "gPosition");
	loc_gNormal = glGetUniformLocation(lightingPass->GetProgram(), "gNormal");
	loc_gAlbedo = glGetUniformLocation(lightingPass->GetProgram(), "gAlbedo");
	loc_shadows = glGetUniformLocation(lightingPass->GetProgram(), "shadows");
	loc_ambientTextures = glGetUniformLocation(lightingPass->GetProgram(), "ambientTextures");
	loc_texMatrices = glGetUniformLocation(lightingPass->GetProgram(), "texMatrices");
	loc_numXTiles = glGetUniformLocation(lightingPass->GetProgram(), "numXTiles");
	loc_numYTiles = glGetUniformLocation(lightingPass->GetProgram(), "numYTiles");
	loc_camMatrix = glGetUniformLocation(lightingPass->GetProgram(), "camMatrix");

	loc_numShadowCastingLights =
		glGetUniformLocation(lightingPass->GetProgram(), "numShadowCastingLights");
}

void BPLighting::Apply()
{
	if (window->GetKeyboard()->KeyTriggered(KEYBOARD_T))
	{
		renderTiles = !renderTiles;
	}

	LightingPass();
}

void BPLighting::LightingPass()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	SetCurrentShader(lightingPass);

	glUniform1i(glGetUniformLocation(lightingPass->GetProgram(), "renderTiles"), renderTiles);

	glUniform1f(glGetUniformLocation(lightingPass->GetProgram(), "nearPlane"), GLConfig::NEAR_PLANE);
	glUniform1f(glGetUniformLocation(lightingPass->GetProgram(), "farPlane"), GLConfig::FAR_PLANE);
	glUniform1i(loc_gPosition, GLConfig::GPOSITION);
	glUniform1i(loc_gNormal, GLConfig::GNORMAL);
	glUniform1i(loc_gAlbedo, GLConfig::GALBEDO);
	glUniform1f(glGetUniformLocation(lightingPass->GetProgram(), "ambientLighting"), ambientLighting);

	glUniform1i(loc_numXTiles, GLConfig::NUM_X_AXIS_TILES);
	glUniform1i(loc_numYTiles, GLConfig::NUM_Y_AXIS_TILES);

	glUniform1iv(loc_ambientTextures, numAmbTex, ambientTextures->texUnits);

	viewMatrix = camera->BuildViewMatrix();
	glUniformMatrix4fv(loc_camMatrix, 1, false, (float*)&viewMatrix);

	Vector3 camPos = camera->GetPosition();
	float vec4[4] = { camPos.x, camPos.y, camPos.z, 0 };
	glUniform4fv(glGetUniformLocation(lightingPass->GetProgram(), "cameraPos"), 1, vec4);

	UpdateShaderMatrices();

	currentShader->ApplyTexture(GLConfig::GPOSITION, *gBuffer->gPosition);
	currentShader->ApplyTexture(GLConfig::GNORMAL, *gBuffer->gNormal);
	currentShader->ApplyTexture(GLConfig::GALBEDO, *gBuffer->gAlbedo);

	RenderScreenQuad();
}