#include "ComputeShader.h"
#include "../Game/Utility/Util.h"

#include <string>
#include <algorithm>
#include <vector>
#include <sstream>
#include <fstream>
#include <iterator>

ComputeShader::ComputeShader(string compute, bool isVerbose)
{
	verbose = isVerbose;

	//Store them in case regen/linking is needed.
	this->compute = compute;

	program = glCreateProgram();
	object[0] = GenerateShader(compute);

	glAttachShader(program, object[0]);
}


ComputeShader::~ComputeShader()
{
	glDetachShader(program, object[0]);
	glDeleteShader(object[0]);
	glDeleteProgram(program);
}

void ComputeShader::Regenerate() {
	program = glCreateProgram();

	object[0] = GenerateShader(compute);

	if (!compute.empty()) {
		object[0] = GenerateShader(compute);
		glAttachShader(program, object[0]);
	}

	glAttachShader(program, object[0]);
}

void ComputeShader::UseProgram()
{
	glUseProgram(program);
}

void ComputeShader::Compute(Vector3 workGroups)
{
	glDispatchCompute(workGroups.x, workGroups.y, workGroups.z);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);
}

GLuint ComputeShader::GenerateShader(string from)
{
	if (verbose) cout << " Compiling Shader ... " << endl;

	string load;
	if (!LoadShaderFile(from, load)) 
	{
		if (verbose) cout << "Compiling failed !" << endl;
		loadFailed = true;
		return 0;
	}

	GLuint shader = glCreateShader(GL_COMPUTE_SHADER);

	const char* chars = load.c_str();
	glShaderSource(shader, 1, &chars, NULL);
	glCompileShader(shader);

	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

	if (status == GL_FALSE) 
	{
		if (verbose) cout << "Compiling failed !" << endl;

		char error[512];
		glGetInfoLogARB(shader, sizeof(error), NULL, error);

		if (verbose) cout << error;

		loadFailed = true;
		return 0;
	}

	if (verbose) cout << "Compiling success !" << endl << endl;

	loadFailed = false;
	return shader;
}

bool ComputeShader::LoadShaderFile(string from, string & into) {
	ifstream file;
	string temp;

	if (verbose) cout << "Loading shader text from " << from << endl << endl;

	file.open(from.c_str());
	if (!file.is_open()) {
		if (verbose) cout << "File does not exist !" << endl;
		return false;
	}
	while (!file.eof()) {
		getline(file, temp);

		if (temp.find("#include") != std::string::npos)
		{
			into += IncludeShader(temp) + "\n";
		}
		else
		{
			into += temp + "\n";
		}
	}

	file.close();
	if (verbose) cout << "Loaded shader text !" << endl << endl;
	return true;
}

string ComputeShader::IncludeShader(string includeLine)
{
	std::istringstream iss(includeLine);

	vector<string> tokens{ istream_iterator<string>{iss},
		istream_iterator<string>{} };

	string glslToAppend;
	LoadShaderFile(tokens.at(1), glslToAppend);

	return glslToAppend;
}

bool ComputeShader::LinkProgram() {
	if (loadFailed) {
		return false;
	}
	glLinkProgram(program);

	GLint code;
	glGetProgramiv(program, GL_LINK_STATUS, &code);
	return code == GL_TRUE ? true : false;
}
