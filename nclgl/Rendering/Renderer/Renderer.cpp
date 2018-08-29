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
	defaultLights[0] = new Light(Vector3(0, 1800, -2000), Vector4(0.9, 0.7, 0.4, 1), 1000000.0f, 1.0f, 0.5f);
	defaultLights[1] = new Light(Vector3(-630, 140, -150), Vector4(1.0f, (140.0f / 255.0f), 0.0f, 1), 150.0f, 1.0f, 1.0f);
	defaultLights[2] = new Light(Vector3(500, 140, -200), Vector4(1.0f, (140.0f / 255.0f), 0.0f, 1), 150.0f, 1.0f, 1.0f);
	defaultLights[3] = new Light(Vector3(-630, 140, 150), Vector4(1.0f, (140.0f / 255.0f), 0.0f, 1), 150.0f, 1.0f, 1.0f);
	defaultLights[4] = new Light(Vector3(500, 140, 150), Vector4(1.0f, (140.0f / 255.0f), 0.0f, 1), 150.0f, 1.0f, 1.0f);

	for (int i = 5; i < 10; i++)
	{
		defaultLights[i] = new Light(Vector3(-3500 + (500 * i), 500, 450), Vector4(1, 0, 0, 1), 270.0f, 3.0f, 2.0f);
	}

	for (int i = 10; i < 15; i++)
	{
		defaultLights[i] = new Light(Vector3(-3500 + (500 * (i - 5)), 500, -450), Vector4(0, 1, 0, 1), 270.0f, 3.0f, 2.0f);
	}

	for (int i = 15; i < 20; i++)
	{
		defaultLights[i] = new Light(Vector3(-3500 + (500 * (i - 10)), 100, 450), Vector4(0, 0, 1, 1), 270.0f, 3.0f, 2.0f);
	}

	for (int i = 20; i < 25; i++)
	{
		defaultLights[i] = new Light(Vector3(-3500 + (500 * (i - 15)), 100, -450), Vector4(1, 1, 0, 1), 270.0f, 3.0f, 2.0f);
	}

	for (int i = 25; i < 50; i++)
	{
		defaultLights[i] = new Light(Vector3(-1300 + (75 * (i - 20)), 1000, 150), Vector4(1, 0, 1, 1), 75.0f, 3.0f, 2.0f);
	}

	for (int i = 50; i < 75; i++)
	{
		defaultLights[i] = new Light(Vector3(-1300 + (75 * (i - 45)), 1000, -200), Vector4(0, 1, 1, 1), 75.0f, 3.0f, 2.0f);
	}

	for (int i = 75; i < 100; i++)
	{
		defaultLights[i] = new Light(Vector3(-1300 + (75 * (i - 70)), 50, 150), Vector4(1, 0.5, 0, 1), 100.0f, 1.0f, 2.0f);
	}

	for (int i = 100; i < 200; i++)
	{
		defaultLights[i] = new Light(Vector3(-1300 + (75 * (i - 70)), 50, 150), Vector4(1, 0.5, 0, 1), 100.0f, 1.0f, 2.0f);
	}

	textShader = new Shader(SHADERDIR"TexturedVertex.glsl", SHADERDIR"TexturedFragment.glsl");
	SetCurrentShader(textShader);

	SetAsDebugDrawingRenderer(); //For light debugging

	tiles = new TileRenderer(lights, GLConfig::NUM_LIGHTS,
		GLConfig::NUM_X_AXIS_TILES, GLConfig::NUM_Y_AXIS_TILES, GLConfig::NUM_Z_AXIS_TILES,
		GLConfig::MIN_NDC_COORDS, GLConfig::MAX_NDC_COORDS, camera);

	screenTiles = tiles->GetScreenTiles();
	tileData = tiles->GetTileData();
	tiles->dataPrepWorkGroups = &currentScenesDataPrepWorkGroups;

	InitLightSSBO();

	glClearColor(0.0f, 1.0f, 0.0f, 1.0f);

	profilerTextRenderer = new TextRenderer(this);
	GLUtil::CheckGLError("Renderer Initialisation");
	init = true;
	sceneIndex = -1;
	glViewport(0, 0, GLConfig::RESOLUTION.x, GLConfig::RESOLUTION.y);
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
	delete profilerTextRenderer;
}

void Renderer::InitLightSSBO()
{
	GLUtil::ClearGLErrorStack();

	ssbo = GLUtil::InitSSBO(1, 1, ssbo,
		sizeof(LightData) * GLConfig::NUM_LIGHTS, &lightData, GL_STATIC_COPY);
	GLUtil::CheckGLError("Light Data SSBO");

	glGenBuffers(1, &modelMatricesSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, modelMatricesSSBO);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 8, modelMatricesSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

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
		profilerEnabled = !profilerEnabled;
	}

	if (wparent->GetKeyboard()->KeyTriggered(KEYBOARD_O))
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
}

void Renderer::RenderScene()
{
	BuildMeshLists();
	SortMeshLists();

	for (int i = 0; i < GComponents.size(); ++i)
	{
		GComponents[i]->Apply();
	}

	if (profilerEnabled)
	{
		if (!profilerTextRenderer->textbuffer.empty())
		{
			glDisable(GL_DEPTH_TEST);
			DrawProfilerText();
			profilerTextRenderer->textbuffer.clear();
		}

		if (debugMode)
		{
			DrawDebugLights();
		}

		glEnable(GL_DEPTH_TEST);
	}
	
	SwapBuffers();
	ClearMeshLists();

	updateTimer.StopTimer();
}

void Renderer::ChangeScene()
{
	models = scenes[sceneIndex]->GetModels();

	for each (Model* model in *models)
	{
		for each (ModelMesh* mesh in model->meshes) 
		{
			mesh->modelMatricesSSBO = modelMatricesSSBO;
		}
	}

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
	}

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(LightData) * GLConfig::NUM_LIGHTS, 
		&lightData, GL_STATIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	lighting->ambientLighting = scenes[sceneIndex]->ambient;
}

void Renderer::UpdateScene(const float& msec)
{
	if (!pauseAuto)
	{
		msUntilSceneChange -= msec;
	}

	if (msUntilSceneChange <= 0)
	{
		autoChangeScene = false;
	}

	if (wparent->GetKeyboard()->KeyTriggered(KEYBOARD_UP))
	{
		pauseAuto = !pauseAuto;
	}

	if (wparent->GetKeyboard()->KeyTriggered(KEYBOARD_LEFT))
	{
		--sceneIndex;

		if (sceneIndex < 0)
		{
			sceneIndex = scenes.size() - 1;
		}

		ChangeScene();
	}
	else if (wparent->GetKeyboard()->KeyTriggered(KEYBOARD_RIGHT) || autoChangeScene)
	{
		autoChangeScene = false;
		msUntilSceneChange = MAX_MS_UNTIL_SCENE_CHANGE;
		++sceneIndex;

		if (sceneIndex == scenes.size())
		{
			sceneIndex = 0;
		}

		ChangeScene();
	}

	currentScenesDataPrepWorkGroups = scenes[sceneIndex]->lightWorkGroups;
	scenes[sceneIndex]->UpdateScene(msec);

	for each (int modifiedLightIndex in scenes[sceneIndex]->modifiedLights)
	{
		lightData[modifiedLightIndex] = lights[modifiedLightIndex]->GetData();

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, sizeof(LightData) * modifiedLightIndex, sizeof(LightData), &lightData[modifiedLightIndex]);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}

	scenes[sceneIndex]->modifiedLights.clear();

	camera->UpdateCamera(msec);
	viewMatrix = camera->BuildViewMatrix();

	frameFrustum.FromMatrix(GLConfig::SHARED_PROJ_MATRIX * viewMatrix);
}

void Renderer::DrawDebugLights()
{
	Vector3 workGroups = *scenes[sceneIndex]->lightWorkGroups;
	int numLights = workGroups.x * workGroups.y * workGroups.z;

	//Prepare everything to render transparent spheres + debug shapes
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Set a shader
	SetCurrentShader(debugSphereShader);

	glDisable(GL_BLEND);

	for (int i = 0; i < numLights; ++i)
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

	tiles->dataPrep->Regenerate();
	tiles->dataPrep->LinkProgram();

	tiles->compute->Regenerate();
	tiles->compute->LinkProgram();
}

void Renderer::DrawProfilerText() const
{
	profilerTextRenderer->DrawTextBuffer();

	currentShader->LinkProgram();
	glUseProgram(currentShader->GetProgram());
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

				if (frameFrustum.InsideFrustum(position, radius))
				{
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
				}
			}
		}
	}
}

void Renderer::SortMeshLists()
{
	std::sort(modelsInFrame.begin(),modelsInFrame.end(), [](const ModelMesh* a, const ModelMesh* b)
	{
		return a->GetDistanceFromCamera() > b->GetDistanceFromCamera();
	});

	std::sort(transparentModelsInFrame.begin(),
		transparentModelsInFrame.end(),
		ModelMesh::CompareByCameraDistance);
}

void Renderer::ClearMeshLists()
{
	modelsInFrame.clear();
	transparentModelsInFrame.clear();
}