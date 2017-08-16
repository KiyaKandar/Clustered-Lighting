#pragma once

#include "../nclgl/GSetting.h"

class GLUtil
{
public:

	template <class T>
	static GLuint InitSSBO(int numBuffers, int binding, GLuint bufferID, size_t size, T* data, GLenum usage)
	{
		glGenBuffers(numBuffers, &bufferID);

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, bufferID);
		glBufferData(GL_SHADER_STORAGE_BUFFER, size, data, usage);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, bufferID);

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

		return bufferID;
	}

	template <class T>
	static GLuint RebufferData(GLenum target, GLuint bufferID, size_t offset, size_t size, T* data)
	{
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, bufferID);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, size, data);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

		return bufferID;
	}

	static void CheckGLError(std::string tag)
	{
		GLenum err;
		while ((err = glGetError()) != GL_NO_ERROR) {
			//cerr << "OpenGL error: " << err << endl;

			string error;

			switch (err) {
			case GL_INVALID_OPERATION:      error = "INVALID_OPERATION";      break;
			case GL_INVALID_ENUM:           error = "INVALID_ENUM";           break;
			case GL_INVALID_VALUE:          error = "INVALID_VALUE";          break;
			case GL_OUT_OF_MEMORY:          error = "OUT_OF_MEMORY";          break;
			case GL_INVALID_FRAMEBUFFER_OPERATION:  error = "INVALID_FRAMEBUFFER_OPERATION";  break;
			}

			cerr << tag << "  -  GL_" << error.c_str() << endl;
			err = glGetError();
		}
	}

	static void ClearGLErrorStack()
	{
		GLenum err;
		while ((err = glGetError()) != GL_NO_ERROR)
		{
			err = glGetError();
		}
	}

};

