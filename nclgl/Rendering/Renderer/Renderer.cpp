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
	//lights[0] = new Light(Vector3(0, 700, -10), Vector4(1, 1, 1, 1), 2000.0f, 10.5f, Vector4(0, -1, 0, 1));
	lights[0] = new Light(Vector3(0, 1800, 200), Vector4(0.9, 0.7, 0.4, 1), 30000.0f, 0.5f);
	lights[1] = new Light(Vector3(-630, 140, -200), Vector4(1.0f, (140.0f / 255.0f), 0.0f, 1), 150.0f, 1.0f);
	lights[2] = new Light(Vector3(500, 140, -200), Vector4(1.0f, (140.0f / 255.0f), 0.0f, 1), 150.0f, 1.0f);
	lights[3] = new Light(Vector3(-630, 140, 150), Vector4(1.0f, (140.0f / 255.0f), 0.0f, 1), 150.0f, 1.0f);
	lights[4] = new Light(Vector3(500, 140, 150), Vector4(1.0f, (140.0f / 255.0f), 0.0f, 1), 150.0f, 1.0f);

	for (int i = 5; i < 10; i++)
	{
		lights[i] = new Light(Vector3(-3500 + (500 * i), 500, 450), Vector4(1, 0, 0, 1), 270.0f, 1.0f);
	}

	for (int i = 10; i < 15; i++)
	{
		lights[i] = new Light(Vector3(-3500 + (500 * (i - 5)), 500, -450), Vector4(0, 1, 0, 1), 270.0f, 1.0f);
	}

	for (int i = 15; i < 20; i++)
	{
		lights[i] = new Light(Vector3(-3500 + (500 * (i - 10)), 100, 450), Vector4(0, 0, 1, 1), 270.0f, 1.0f);
	}

	for (int i = 20; i < 25; i++)
	{
		lights[i] = new Light(Vector3(-3500 + (500 * (i - 15)), 100, -450), Vector4(1, 1, 0, 1), 270.0f, 1.0f);
	}

	for (int i = 25; i < 50; i++)
	{
		lights[i] = new Light(Vector3(-1300 + (75 * (i - 20)), 1000, 150), Vector4(1, 0, 1, 1), 75.0f, 1.0f);
	}

	for (int i = 50; i < 75; i++)
	{
		lights[i] = new Light(Vector3(-1300 + (75 * (i - 45)), 1000, -200), Vector4(0, 1, 1, 1), 75.0f, 1.0f);
	}

	for (int i = 75; i < 100; i++)
	{
		lights[i] = new Light(Vector3(-1300 + (75 * (i - 70)), 50, 150), Vector4(1, 0.5, 0, 1), 100.0f, 0.5f);
	}

	//lights[99] = new Light(Vector3(100, 150, 100), Vector4(1, 0, 0, 1), 500, 0.5f);

	//Initialise text for the profiler
	textShader = new Shader(SHADERDIR"TexturedVertex.glsl", SHADERDIR"TexturedFragment.glsl");
	basicFont = new Font(SOIL_load_OGL_texture(TEXTUREDIR"tahoma.tga",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_COMPRESS_TO_DXT), 16, 16);

	InitDebugLights();

	SetAsDebugDrawingRenderer(); //For light debugging
	SetCurrentShader(textShader);

	for (int i = 0; i < GLConfig::NUM_LIGHTS; ++i)
	{
		lightData[i] = lights[i]->GetData();
		spotLightData[i] = lights[i]->GetSpotData();
	}

	tiles = new TileRenderer(lights, GLConfig::NUM_LIGHTS,
		GLConfig::NUM_X_AXIS_TILES, GLConfig::NUM_Y_AXIS_TILES, GLConfig::NUM_Z_AXIS_TILES,
		GLConfig::MIN_NDC_COORDS, GLConfig::MAX_NDC_COORDS);

	screenTiles = tiles->GetScreenTiles();
	tileData = tiles->GetTileData();

	InitLightSSBO();

	glClearColor(0.f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	textRenderer = new TextRenderer(this);
	glEnable(GL_MULTISAMPLE);
	GLUtil::CheckGLError("Renderer Initialisation");
	init = true;
}

Renderer::~Renderer()
{
	delete camera;

	for each (Light* light in lights)
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
	for (int i = 1; i < GLConfig::NUM_LIGHTS; ++i)
	{
		//Create new sphere.
		Model* sphere = new Model("../sphere/sphere.obj");

		//Set size and position to match light.
		sphere->Translate(lights[i]->GetPosition());

		const float radius = lights[i]->GetRadius();
		sphere->Scale(Vector3(radius, radius, radius));

		//Add it to a seperate list.
		debugSpheres.push_back(sphere);
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

	const int numComponents = GComponents.size();
	for (int i = 0; i < numComponents; ++i)
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

void Renderer::UpdateScene(const float& msec)
{
	if (wparent->GetKeyboard()->KeyTriggered(KEYBOARD_T))
	{
		++sceneIndex;

		if (sceneIndex == scenes.size())
		{
			sceneIndex = 0;
		}

		models = scenes[sceneIndex]->GetModels();
	}

	camera->UpdateCamera(msec);
	viewMatrix = camera->BuildViewMatrix();

	frameFrustum.FromMatrix(projMatrix * viewMatrix);
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
	glDisable(GL_BLEND);
}

void Renderer::BuildMeshLists()
{
	const int numModels = models->size();

	for (int mod = 0; mod < numModels; ++mod)
	{
		const int numMeshes = (*models)[mod]->meshes.size();

		for (int mes = 0; mes < numMeshes; ++mes)
		{
			//if (frameFrustum.InsideFrustum(models[mod]->meshes[mes]->box))
			//if(frameFrustum.InsideFrustum(models[mod]->meshes[mes]->GetTransform()->GetPositionVector(), models[mod]->meshes[mes]->GetBoundingRadius()))
			//{
				const Vector3 dir = (*models)[mod]->meshes[mes]->GetTransform()->GetPositionVector() -
					camera->GetPosition();
				(*models)[mod]->meshes[mes]->SetCameraDistance(Vector3::Dot(dir, dir));

				modelsInFrame.push_back((*models)[mod]->meshes[mes]);
			//}
		}
	}
}

void Renderer::SortMeshLists()
{
	std::sort(modelsInFrame.begin(),
		modelsInFrame.end(),
		ModelMesh::CompareByCameraDistance);
}

void Renderer::ClearMeshLists()
{
	modelsInFrame.clear();
}