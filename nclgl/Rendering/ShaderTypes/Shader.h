#pragma once

/*
	AUTHOR: RICH DAVISON
*/

#include "../Renderer/OGLRenderer.h"

#include <vector>

#define SHADER_VERTEX	0
#define SHADER_FRAGMENT 1
#define SHADER_GEOMETRY 2

using namespace std;

class Shader
{
public:
	Shader(string vertex, string fragment, 
		string geometry = "", bool isVerbose = false);
	~Shader(void);

	GLuint	GetProgram() { return program; }
	bool	LinkProgram();

	/*
	  Recompile the shaders without having to restart. 
	  (Will still need linking)
	*/
	void Regenerate();

	void ApplyTexture(int unit, unsigned int texture)
	{
		glActiveTexture(GL_TEXTURE0 + unit);
		glBindTexture(GL_TEXTURE_2D, texture);
	}

	void SetInt(const std::string &name, int value) const
	{
		glUniform1i(glGetUniformLocation(program, name.c_str()), value);
	}

	void SetVec3(const std::string &name, const Vector3 &value) const
	{
		glUniform3fv(glGetUniformLocation(program, name.c_str()), 1, (float*)&value);
	}

	void SetVec3(const std::string &name, float x, float y, float z) const
	{
		glUniform3f(glGetUniformLocation(program, name.c_str()), x, y, z);
	}

protected:
	void	SetDefaultAttributes();
	bool	LoadShaderFile(string from, string & into);
	string IncludeShader(string includeLine);
	GLuint	GenerateShader(string from, GLenum type);

	GLuint	objects[3];
	GLuint	program;

	bool	loadFailed;
	bool	verbose; //I added this stuff (Kiya)

	string vFile;
	string fFile;
	string gFile;
};

