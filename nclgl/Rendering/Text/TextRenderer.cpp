#include "TextRenderer.h"

#include "../Renderer/Renderer.h"

TextRenderer::TextRenderer(Renderer* renderer)
{
	this->renderer = renderer;

	textShader = new Shader(SHADERDIR"TexturedVertex.glsl",
		SHADERDIR"TexturedFragment.glsl");
	font = new Font(SOIL_load_OGL_texture(TEXTUREDIR"tahoma.tga",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_COMPRESS_TO_DXT), 16, 16);

	textTextureMatrix.ToIdentity();
}

TextRenderer::~TextRenderer()
{}

void TextRenderer::DrawTextBuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	textShader->LinkProgram();
	glUseProgram(textShader->GetProgram());

	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glEnable(GL_BLEND);

	for each (Text textobj in textbuffer)
	{
		DrawTextOBJ(textobj);
	}

	glDisable(GL_BLEND);
}

void TextRenderer::DrawTextOBJ(const Text& textobj)
{
	TextMesh mesh(textobj.text, *font);

	if (textobj.perspective)
	{
		AssignPerspectiveMatrices(textobj);
	}
	else
	{
		AssignOrthographicMatrices(textobj);
	}

	UpdateUniforms();

	mesh.Draw();
}

void TextRenderer::AssignPerspectiveMatrices(const Text& textobj)
{
	textModelMatrix = Matrix4::Translation(textobj.position) *
		Matrix4::Scale(Vector3(textobj.size, textobj.size, 1));

	textViewMatrix = renderer->GetCamera()->BuildViewMatrix();
	textProjectionMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)renderer->GetWidth() / (float)renderer->GetHeight(), 45.0f);
}

void TextRenderer::AssignOrthographicMatrices(const Text& textobj)
{
	//In ortho mode, we subtract the y from the height, so that a height of 0
	//is at the top left of the screen, which is more intuitive

	const Vector3 position(textobj.position.x, renderer->GetHeight() - textobj.position.y, textobj.position.z);
	const Vector3 size(textobj.size, textobj.size, 1);

	textModelMatrix = Matrix4::Translation(position) * Matrix4::Scale(size);

	textViewMatrix.ToIdentity();

	textProjectionMatrix = Matrix4::Orthographic(-1.0f, 1.0f,
		static_cast<float>(renderer->GetWidth()), 0.0f, static_cast<float>(renderer->GetHeight()), 0.0f);
}

void TextRenderer::UpdateUniforms()
{
	glUniformMatrix4fv(glGetUniformLocation(textShader->GetProgram(), "modelMatrix"), 1, false, (float*)&textModelMatrix);
	glUniformMatrix4fv(glGetUniformLocation(textShader->GetProgram(), "viewMatrix"), 1, false, (float*)&textViewMatrix);
	glUniformMatrix4fv(glGetUniformLocation(textShader->GetProgram(), "projMatrix"), 1, false, (float*)&textProjectionMatrix);
	glUniformMatrix4fv(glGetUniformLocation(textShader->GetProgram(), "textureMatrix"), 1, false, (float*)&textTextureMatrix);
}