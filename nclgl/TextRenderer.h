#pragma once

#include "Text.h"
#include "TextMesh.h"
#include "Shader.h"
#include "Matrix4.h"

#include <vector>

class Renderer;
class TextRenderer
{
public:
	explicit TextRenderer(Renderer* renderer);
	~TextRenderer();

	void DrawTextBuffer();

	std::vector<Text> textbuffer;
private:
	void DrawTextOBJ(const Text& textobj);

	void AssignPerspectiveMatrices(const Text& textobj);
	void AssignOrthographicMatrices(const Text& textobj);

	void UpdateUniforms();

	Font* font;
	Shader* textShader;
	Renderer* renderer;

	Matrix4 textViewMatrix;
	Matrix4 textProjectionMatrix;
	Matrix4 textModelMatrix;
	Matrix4 textTextureMatrix;
};

