#include "Renderer.h"
#include "../Game/Util.h"

Renderer::Renderer(Window &parent, Camera* cam) : OGLRenderer(parent)
{
	camera = cam;
	wparent = &parent;

	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);

	lights[0] = new Light(Vector3(0, 1800, 200),	Vector4(0.9, 0.7, 0.4, 1),					30000.0f,	2.0f);
	lights[1] = new Light(Vector3(-630, 140, -200), Vector4(1.0f, (140.0f / 255.0f), 0.0f, 1),	150.0f,		3.0f);
	lights[2] = new Light(Vector3(500, 140, -200),	Vector4(1.0f, (140.0f / 255.0f), 0.0f, 1),	150.0f,		3.0f);
	lights[3] = new Light(Vector3(-630, 140, 150),	Vector4(1.0f, (140.0f / 255.0f), 0.0f, 1),	150.0f,		3.0f);
	lights[4] = new Light(Vector3(500, 140, 150),	Vector4(1.0f, (140.0f / 255.0f), 0.0f, 1),	150.0f,		3.0f);

	for (int i = 5; i < 10; i++)
	{
		lights[i] = new Light(Vector3(-3000 + (500 * i), 500, 450), Vector4(1, 0, 0, 1), 400.0f, 3.0f);
	}

	//Initialise text for the profiler
	textShader = new Shader(SHADERDIR"TexturedVertex.glsl", SHADERDIR"TexturedFragment.glsl");
	basicFont = new Font(SOIL_load_OGL_texture(TEXTUREDIR"tahoma.tga",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_COMPRESS_TO_DXT), 16, 16);

	InitDebugLights();

	SetAsDebugDrawingRenderer(); //For light debugging
	SetCurrentShader(textShader);

	for (int i = 0; i < NUM_LIGHTS; ++i)
	{
		lightData[i] = lights[i]->GetData();
	}

	tiles = new TileRenderer(lights, NUM_LIGHTS, 
		NUM_X_AXIS_TILES, NUM_Y_AXIS_TILES, NUM_Z_AXIS_TILES,
		MIN_NDC_COORDS, MAX_NDC_COORDS);
	tiles->GenerateGrid();

	screenTiles = tiles->GetScreenTiles();
	//tileLights = tiles->GetTileLights();
	tileData = tiles->GetTileData();

	InitLightSSBO();

	glClearColor(0.f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	init = true;
}

Renderer::~Renderer(void)
{
	delete camera;
	
	for each (Light* light in lights)
	{
		delete light;
	}

	delete tiles;
	//delete basicFont;/*
	//delete textShader;
	//delete sceneShader;
	//delete shadowShader;*/
}

void Renderer::InitDebugLights() {
	for (int i = 1; i < NUM_LIGHTS; ++i) {
		//Create new sphere.
		Model* sphere = new Model("../sphere/sphere.obj");
	
		//Set size and position to match light.
		sphere->Translate(lights[i]->GetPosition());

		float radius = lights[i]->GetRadius();
		sphere->Scale(Vector3(radius, radius, radius));

		//Add it to a seperate list.
		debugSpheres.push_back(sphere);
	}
}

void Renderer::InitLightSSBO()
{
	Util::CheckGLError("renderer nothing");
	glGenBuffers(1, &ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(LightData) * NUM_LIGHTS, &lightData, GL_STATIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	Util::CheckGLError("renderer light data");

	glGenBuffers(1, &tilesssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, tilesssbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Tile) * tiles->GetNumTiles(), screenTiles, GL_STATIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, tilesssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	Util::CheckGLError("renderer tiles");

	glGenBuffers(1, &tilelightssssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, tilelightssssbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(TileData), tileData, GL_STATIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, tilelightssssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	Util::CheckGLError("renderer tile lights");
}

void Renderer::Update(float deltatime)
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

	UpdateScene(deltatime);

	Util::ClearGLErrorStack();
	tiles->PrepareData(projMatrix, viewMatrix);
	tiles->FillTiles();

	tileData = tiles->GetTileData();

	//Rebuffer the data
	Util::CheckGLError("renderer tilelights nothing");
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, tilelightssssbo);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(TileData), tileData);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	Util::CheckGLError("renderer tilelights");

	RenderScene();

	updateTimer.StopTimer();
}

void Renderer::UpdateBasicUniforms()
{
	glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "resolutionY"), height);
	glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "resolutionX"), width);
}

void Renderer::RenderScene()
{
	BuildMeshLists();
	SortMeshLists();

	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	for (std::vector<GSetting*>::const_iterator iter = GComponents.begin(); iter != GComponents.end(); ++iter)
	{
		(*iter)->Apply();
	}

	if (debugMode) 
	{
		if (!textbuffer.empty())
		{
			glDisable(GL_DEPTH_TEST);
			DrawTextBuffer();
			textbuffer.clear();
		}

		DrawDebugLights();

		SwapBuffers();

		glEnable(GL_DEPTH_TEST);
	}
	else SwapBuffers();

	ClearMeshLists();
}

void Renderer::DrawDebugLights()
{
	for (int i = 0; i < NUM_LIGHTS - 1; ++i)
	{
		//Prepare everything to render transparent spheres + debug shapes
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
		UpdateShaderMatrices();
		DrawDebugSphere(debugSpheres[i]);

		glDisable(GL_BLEND);
	}

	for (int i = 0; i < NUM_LIGHTS; ++i) 
	{
		//Blue centre
		DrawDebugCross(DEBUGDRAW_PERSPECTIVE, lights[i]->GetPosition(), Vector3(100, 100, 100), Vector3(0, 0, 1));

		DrawDebugBox(DEBUGDRAW_PERSPECTIVE, lights[i]->GetPosition(), Vector3(100, 100, 0), Vector3(0, 1, 0));
	}
}

void Renderer::UpdateScene(float msec)
{
	camera->UpdateCamera(msec);
	viewMatrix = camera->BuildViewMatrix();

	frameFrustum.FromMatrix(projMatrix * viewMatrix);
}

void Renderer::RelinkShaders()
{
	for each (GSetting* component in GComponents)
	{
		component->RegenerateShaders();
		component->LinkShaders();
	}
}

void Renderer::DrawTextBuffer()
{
	//Store the old values (so we can switch back).
	Matrix4 oldViewmatrix = viewMatrix;
	Matrix4 oldProjmatrix = projMatrix;

	//Use text shader
	Shader* oldshader = currentShader;
	currentShader = textShader;
	currentShader->LinkProgram();

	glUseProgram(currentShader->GetProgram());
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glEnable(GL_BLEND);
	textureMatrix.ToIdentity();

	for each (Text textobj in textbuffer)
	{
		DrawTextOBJ(textobj);
	}

	//Revert to the old matrices
	viewMatrix = oldViewmatrix;
	projMatrix = oldProjmatrix;

	//Switch shader back
	currentShader = oldshader;
	currentShader->LinkProgram();

	glUseProgram(currentShader->GetProgram());
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_BLEND);

}

/*<Author: Richard Davison--->*/
void Renderer::DrawTextOBJ(const Text& textobj)
{
	TextMesh mesh(textobj.text, *basicFont);

	//This just does simple matrix setup to render in either perspective or
	//orthographic mode, there's nothing here that's particularly tricky.
	if (textobj.perspective)
	{
		modelMatrix = Matrix4::Translation(textobj.position) *
			Matrix4::Scale(Vector3(textobj.size, textobj.size, 1));

		viewMatrix = camera->BuildViewMatrix();
		projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);
	}
	else
	{
		//In ortho mode, we subtract the y from the height, so that a height of 0
		//is at the top left of the screen, which is more intuitive
		modelMatrix = Matrix4::Translation(Vector3(textobj.position.x,
			height - textobj.position.y,
			textobj.position.z))
			* Matrix4::Scale(Vector3(textobj.size, textobj.size, 1));

		viewMatrix.ToIdentity();
		projMatrix = Matrix4::Orthographic(-1.0f, 1.0f, (float)width, 0.0f, (float)height, 0.0f);
	}

	//Either way, we update the matrices, and draw the mesh
	UpdateShaderMatrices();
	mesh.Draw();
}
/*<Author: by Richard Davison>*/

void Renderer::BuildMeshLists()
{
	for (std::vector<Model*>::const_iterator outerIter = models.begin(); outerIter != models.end(); ++outerIter)
	{
		for (std::vector<ModelMesh*>::const_iterator innerIter = (*outerIter)->meshes.begin(); innerIter != (*outerIter)->meshes.end(); ++innerIter)
		{
			if (frameFrustum.InsideFrustum((*innerIter)->box))
			{
				Vector3 dir = (*innerIter)->GetTransform()->GetPositionVector() -
					camera->GetPosition();
				(*innerIter)->SetCameraDistance(Vector3::Dot(dir, dir));

				modelsInFrame.push_back((*innerIter));
			}
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