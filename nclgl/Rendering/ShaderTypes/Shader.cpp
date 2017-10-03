#include "Shader.h"

#include <vector>
#include <sstream>
#include <iterator>

Shader::Shader(string vFile, string fFile, 
	string gFile, bool isVerbose)
{
	verbose = isVerbose;

	//Store them in case regen/linking is needed.
	this->vFile = vFile;
	this->fFile = fFile;
	this->gFile = gFile;

	program = glCreateProgram();
	objects[SHADER_VERTEX] = GenerateShader(vFile, GL_VERTEX_SHADER);
	objects[SHADER_FRAGMENT] = GenerateShader(fFile, GL_FRAGMENT_SHADER);
	objects[SHADER_GEOMETRY] = 0;

	if (!gFile.empty()) {
		objects[SHADER_GEOMETRY] = GenerateShader(gFile, GL_GEOMETRY_SHADER);
		glAttachShader(program, objects[SHADER_GEOMETRY]);
	}

	glAttachShader(program, objects[SHADER_VERTEX]);
	glAttachShader(program, objects[SHADER_FRAGMENT]);

	SetDefaultAttributes();
}


Shader::~Shader(void)
{
	for (int i = 0; i < 3; ++i) {
		glDetachShader(program, objects[i]);
		glDeleteShader(objects[i]);
	}
	glDeleteProgram(program);
}

void Shader::Regenerate() {
	program = glCreateProgram();
	objects[SHADER_VERTEX] = GenerateShader(vFile, GL_VERTEX_SHADER);
	objects[SHADER_FRAGMENT] = GenerateShader(fFile, GL_FRAGMENT_SHADER);
	objects[SHADER_GEOMETRY] = 0;

	if (!gFile.empty()) {
		objects[SHADER_GEOMETRY] = GenerateShader(gFile, GL_GEOMETRY_SHADER);
		glAttachShader(program, objects[SHADER_GEOMETRY]);
	}

	glAttachShader(program, objects[SHADER_VERTEX]);
	glAttachShader(program, objects[SHADER_FRAGMENT]);

	SetDefaultAttributes();
}

GLuint Shader::GenerateShader(string from, GLenum type) {
	if(verbose) cout << " Compiling Shader ... " << endl;

	string load;
	if (!LoadShaderFile(from, load)) {
		if (verbose) cout << "Compiling failed !" << endl;
		loadFailed = true;
		return 0;
	}

	GLuint shader = glCreateShader(type);

	const char* chars = load.c_str();
	glShaderSource(shader, 1, &chars, NULL);
	glCompileShader(shader);

	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

	if (status == GL_FALSE) {
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

bool Shader::LoadShaderFile(string from, string & into) {
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
	if (verbose) cout << into << endl << endl;
	if (verbose) cout << "Loaded shader text !" << endl << endl;
	return true;
}

string Shader::IncludeShader(string includeLine)
{
	std::istringstream iss(includeLine);

	vector<string> tokens{ istream_iterator<string>{iss},
		istream_iterator<string>{} };

	string glslToAppend;
	LoadShaderFile(tokens.at(1), glslToAppend);

	return glslToAppend;
}

void Shader::SetDefaultAttributes() {
	glBindAttribLocation(program, VERTEX_BUFFER, "position");
	glBindAttribLocation(program, COLOUR_BUFFER, "colour");
	glBindAttribLocation(program, NORMAL_BUFFER, "normal");
	glBindAttribLocation(program, TANGENT_BUFFER, "tangent");
	glBindAttribLocation(program, TEXTURE_BUFFER, "texCoord");
}

bool Shader::LinkProgram() {
	if (loadFailed) {
		return false;
	}
	glLinkProgram(program);

	GLint code;
	glGetProgramiv(program, GL_LINK_STATUS, &code);
	return code == GL_TRUE ? true : false;
}
