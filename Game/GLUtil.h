#pragma once

#include "../nclgl/GSetting.h"
#include "GLConfig.h"

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
		glBindBuffer(target, bufferID);
		glBufferSubData(target, offset, size, data);
		glBindBuffer(target, 0);

		return bufferID;
	}

	static void CreateScreenTexture(GLuint textureID, GLuint internalFormat, GLenum format, 
		GLenum type, GLint minMagParam, int attachment, bool clamp)
	{
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, GLConfig::RESOLUTION.x, GLConfig::RESOLUTION.y, 0, format, type, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minMagParam);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, minMagParam);

		if (clamp) 
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		}

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + attachment, GL_TEXTURE_2D, textureID, 0);
	}

	//If updating all basic uniforms is overkill.
	static void UpdateResolutionUniforms(const GLuint& program)
	{
		glUniform1i(glGetUniformLocation(program, "resolutionX"), GLConfig::RESOLUTION.x);
		glUniform1i(glGetUniformLocation(program, "resolutionY"), GLConfig::RESOLUTION.y);
	}

	/*
	  WARNING
	  Will print entire error stack.
	   - Ensure regular calls to assist debugging.
	   - Or clear the stack using the ClearGLErrorStack function.
	*/
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

			cerr << tag << "  -  GL_" << error.c_str() << "\n";
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

	static void VerifyBuffer(std::string name, bool successMsg) {
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			std::cout << name + " - buffer not complete!" << "\n";
		}
		else if (successMsg)
		{
			std::cout << name + " - buffer intialised succesfully." << "\n";
		}
	}

};

