#include "Renderer.h"
#include "../Game/Utility/Util.h"
#include "../Game/GraphicsConfiguration/GLUtil.h"

#include "../Game/GraphicsConfiguration/GLConfig.h"

Renderer::Renderer(Window &parent, Camera* cam) : OGLRenderer(parent)
{
	camera = cam;
	wparent = &parent;

	projMatrix = GLConfig::SHARED_PROJ_MATRIX;// Matrix4::Perspective(1.0f, 15000.0f, static_cast<float>(width) / static_cast<float>(height), 45.0f);

	//Shadow casting lights must be declared first
	defaultLights[0] = new Light(Vector3(0, 1800, -2000), Vector4(0.9, 0.7, 0.4, 1), 1000000.0f, 0.5f);
	defaultLights[1] = new Light(Vector3(0, 1800, 2000), Vector4(0.9, 0.7, 0.4, 1), 1000000.0f, 0.5f);
	defaultLights[2] = new Light(Vector3(500, 140, -200), Vector4(1.0f, (140.0f / 255.0f), 0.0f, 1), 150.0f, 1.0f);
	defaultLights[3] = new Light(Vector3(-630, 140, 150), Vector4(1.0f, (140.0f / 255.0f), 0.0f, 1), 150.0f, 1.0f);
	defaultLights[4] = new Light(Vector3(500, 140, 150), Vector4(1.0f, (140.0f / 255.0f), 0.0f, 1), 150.0f, 1.0f);

	for (int i = 5; i < 10; i++)
	{
		defaultLights[i] = new Light(Vector3(-3500 + (500 * i), 500, 450), Vector4(1, 0, 0, 1), 270.0f, 1.0f);
	}

	for (int i = 10; i < 15; i++)
	{
		defaultLights[i] = new Light(Vector3(-3500 + (500 * (i - 5)), 500, -450), Vector4(0, 1, 0, 1), 270.0f, 1.0f);
	}

	for (int i = 15; i < 20; i++)
	{
		defaultLights[i] = new Light(Vector3(-3500 + (500 * (i - 10)), 100, 450), Vector4(0, 0, 1, 1), 270.0f, 1.0f);
	}

	for (int i = 20; i < 25; i++)
	{
		defaultLights[i] = new Light(Vector3(-3500 + (500 * (i - 15)), 100, -450), Vector4(1, 1, 0, 1), 270.0f, 1.0f);
	}

	for (int i = 25; i < 50; i++)
	{
		defaultLights[i] = new Light(Vector3(-1300 + (75 * (i - 20)), 1000, 150), Vector4(1, 0, 1, 1), 75.0f, 1.0f);
	}

	for (int i = 50; i < 75; i++)
	{
		defaultLights[i] = new Light(Vector3(-1300 + (75 * (i - 45)), 1000, -200), Vector4(0, 1, 1, 1), 75.0f, 1.0f);
	}

	for (int i = 75; i < 100; i++)
	{
		defaultLights[i] = new Light(Vector3(-1300 + (75 * (i - 70)), 50, 150), Vector4(1, 0.5, 0, 1), 100.0f, 0.5f);
	}

	//lights[99] = new Light(Vector3(100, 150, 100), Vector4(1, 0, 0, 1), 500, 0.5f);

	//Initialise text for the profiler
	textShader = new Shader(SHADERDIR"TexturedVertex.glsl", SHADERDIR"TexturedFragment.glsl");
	basicFont = new Font(SOIL_load_OGL_texture(TEXTUREDIR"tahoma.tga",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_COMPRESS_TO_DXT), 16, 16);

	//InitDebugLights();

	SetAsDebugDrawingRenderer(); //For light debugging
	SetCurrentShader(textShader);

	tiles = new TileRenderer(lights, GLConfig::NUM_LIGHTS,
		GLConfig::NUM_X_AXIS_TILES, GLConfig::NUM_Y_AXIS_TILES, GLConfig::NUM_Z_AXIS_TILES,
		GLConfig::MIN_NDC_COORDS, GLConfig::MAX_NDC_COORDS);

	screenTiles = tiles->GetScreenTiles();
	tileData = tiles->GetTileData();
	tiles->dataPrepWorkGroups = &currentScenesDataPrepWorkGroups;

	InitLightSSBO();

	glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	textRenderer = new TextRenderer(this);
	GLUtil::CheckGLError("Renderer Initialisation");
	init = true;
	sceneIndex = 0;

	debugSpheres = vector<Model*>(GLConfig::NUM_LIGHTS);

	glDepthFunc(GL_LESS);
}

Renderer::~Renderer()
{
	delete camera;

	for each (Light* light in defaultLights)
	{
		delete light;
	}

	for each (Scene* scene in scenes)
	{
		delete scene;
	}

	delete tiles;
	delete textRenderer;
}

void Renderer::InitDebugLights()
{
	for (int i = 0; i < GLConfig::NUM_LIGHTS; ++i)
	{
		delete debugSpheres[i];
		//Create new sphere.
		Model* sphere = new Model("../sphere/sphere.obj", 1);

		//Set size and position to match light.
		sphere->Translate(lights[i]->GetPosition());

		const float radius = lights[i]->GetRadius();
		sphere->Scale(Vector3(radius, radius, radius));

		//Add it to a seperate list.
		debugSpheres[i] = sphere;
	}
}

void Renderer::InitLightSSBO()
{
	GLUtil::ClearGLErrorStack();

	ssbo = GLUtil::InitSSBO(1, 1, ssbo,
		sizeof(LightData) * GLConfig::NUM_LIGHTS, &lightData, GL_STATIC_COPY);
	GLUtil::CheckGLError("Light Data SSBO");

	spotlightssbo = GLUtil::InitSSBO(1, 7, spotlightssbo,
		sizeof(SpotLightData) * GLConfig::NUM_LIGHTS, &spotLightData, GL_STATIC_COPY);
	GLUtil::CheckGLError("SpotLight Data SSBO");

	tilesssbo = GLUtil::InitSSBO(1, 2, tilesssbo,
		sizeof(Tile) * tiles->GetNumTiles(), screenTiles, GL_STATIC_COPY);
	GLUtil::CheckGLError("Screen Tiles SSBO");

	tilelightssssbo = GLUtil::InitSSBO(1, 3, tilelightssssbo,
		sizeof(TileData), tileData, GL_STATIC_COPY);

	GLUtil::CheckGLError("Tile Data SSBO");
}

void Renderer::Update(const float& deltatime)
{
	updateTimer.StartTimer();

	if (wparent->GetKeyboard()->KeyTriggered(KEYBOARD_P))
	{
		debugMode = !debugMode;
	}

	if (wparent->GetKeyboard()->KeyTriggered(KEYBOARD_R))
	{
		RelinkShaders();
	}

	previousViewMatrix = camera->viewMatrix;
	UpdateScene(deltatime);
	currentViewProj = camera->viewMatrix;

	tiles->AllocateLightsGPU(GLConfig::SHARED_PROJ_MATRIX, viewMatrix, camera->GetPosition());

	RenderScene();

	updateTimer.StopTimer();
}

void Renderer::RenderScene()
{
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	BuildMeshLists();
	SortMeshLists();

	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	for (int i = 0; i < GComponents.size(); ++i)
	{
		GComponents[i]->Apply();
	}

	if (debugMode)
	{
		if (!textRenderer->textbuffer.empty())
		{
			glDisable(GL_DEPTH_TEST);
			DrawAllText();
			textRenderer->textbuffer.clear();
		}

		DrawDebugLights();

		SwapBuffers();

		glEnable(GL_DEPTH_TEST);
	}
	else SwapBuffers();

	ClearMeshLists();
}

void Renderer::ChangeScene()
{
	++sceneIndex;

	if (sceneIndex == scenes.size())
	{
		sceneIndex = 0;
	}

	models = scenes[sceneIndex]->GetModels();
	skybox->SetSkyboxTexture(scenes[sceneIndex]->GetSkyboxTextureID());
	gBuffer->SetReflectionTextureID(scenes[sceneIndex]->GetReflectionCubeMapTextureID());

	for (int i = 0; i < GLConfig::NUM_LIGHTS; i++)
	{
		lights[i] = defaultLights[i];
	}

	for each (pair<Light*, int> newLight in scenes[sceneIndex]->GetSceneLights())
	{
		lights[newLight.second] = newLight.first;
	}

	for (int i = 0; i < GLConfig::NUM_LIGHTS; ++i)
	{
		lightData[i] = lights[i]->GetData();
		spotLightData[i] = lights[i]->GetSpotData();
	}

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(LightData) * GLConfig::NUM_LIGHTS, 
		&lightData, GL_STATIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, spotlightssbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(SpotLightData) * GLConfig::NUM_LIGHTS,
		&spotLightData, GL_STATIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 7, spotlightssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	InitDebugLights();
	lighting->UpdateShadowData(scenes[sceneIndex]->GetShadowData());
	particleSystem->particles = &scenes[sceneIndex]->particles;
}

void Renderer::UpdateScene(const float& msec)
{
	if (wparent->GetKeyboard()->KeyTriggered(KEYBOARD_T))
	{
		ChangeScene();
	}

	currentScenesDataPrepWorkGroups = scenes[sceneIndex]->lightWorkGroups;
	scenes[sceneIndex]->UpdateScene(msec);

	camera->UpdateCamera(msec);
	viewMatrix = camera->BuildViewMatrix();

	frameFrustum.FromMatrix(GLConfig::SHARED_PROJ_MATRIX * viewMatrix);
}

void Renderer::DrawDebugLights()
{
	for (int i = 0; i < GLConfig::NUM_LIGHTS - 1; ++i)
	{
		//Prepare everything to render transparent spheres + debug shapes
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		UpdateShaderMatrices();

		DrawDebugSphere(debugSpheres[i]);

		glDisable(GL_BLEND);
	}

	for (int i = 0; i < GLConfig::NUM_LIGHTS; ++i)
	{
		//Blue centre
		DrawDebugCross(DEBUGDRAW_PERSPECTIVE, lights[i]->GetPosition(), Vector3(100, 100, 100), Vector3(0, 0, 1));
		DrawDebugBox(DEBUGDRAW_PERSPECTIVE, lights[i]->GetPosition(), Vector3(100, 100, 0), Vector3(0, 1, 0));
	}
}

void Renderer::RelinkShaders() const
{
	for each (GSetting* component in GComponents)
	{
		component->RegenerateShaders();
		component->LinkShaders();
	}

	skybox->RegenerateShaders();

	tiles->dataPrep->Regenerate();
	tiles->dataPrep->LinkProgram();

	tiles->compute->Regenerate();
	tiles->compute->LinkProgram();
}

void Renderer::DrawAllText() const
{
	textRenderer->DrawTextBuffer();

	currentShader->LinkProgram();

	glUseProgram(currentShader->GetProgram());
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
}

void Renderer::BuildMeshLists()
{
	for (int model = 0; model < models->size(); ++model)
	{
		for (int subMesh = 0; subMesh < (*models)[model]->meshes.size(); ++subMesh)
		{
			for (int i = 0; i < (*models)[model]->numModels; ++i)
			{
				const Vector3 position = (*models)[model]->meshes[subMesh]->GetTransform(i)->GetPositionVector();
				const float radius = (*models)[model]->meshes[subMesh]->GetBoundingRadius();

				//if (frameFrustum.InsideFrustum(position, radius))
				//if (frameFrustum.InsideFrustum((*models)[model]->meshes[subMesh]->box))
				//{
					const Vector3 dir = (*models)[model]->meshes[subMesh]->GetTransform(i)->GetPositionVector() -
						camera->GetPosition();
					(*models)[model]->meshes[subMesh]->SetCameraDistance(Vector3::Dot(dir, dir));

					if ((*models)[model]->meshes[subMesh]->baseColour.w < 1.0f)
					{
						transparentModelsInFrame.push_back((*models)[model]->meshes[subMesh]);
					}
					else
					{
						modelsInFrame.push_back((*models)[model]->meshes[subMesh]);
					}
				//}
			}
		}
	}
}

void Renderer::SortMeshLists()
{
	std::sort(modelsInFrame.begin(),
		modelsInFrame.end(),
		ModelMesh::CompareByCameraDistance);

	std::sort(transparentModelsInFrame.begin(),
		transparentModelsInFrame.end(),
		ModelMesh::CompareByCameraDistance);
}

void Renderer::ClearMeshLists()
{
	modelsInFrame.clear();
	transparentModelsInFrame.clear();
}