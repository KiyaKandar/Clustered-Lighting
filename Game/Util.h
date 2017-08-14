#pragma once

#include <stdexcept>

class Util
{
public:

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

	template<typename T>
	static const bool AssertEquals(const T& lhs, const T& rhs, bool throwException = false)
	{
		bool equals = (lhs == rhs);

		if (throwException && !equals)
		{
			throw std::invalid_argument(lhs + " ! equal to " + rhs);
		}

		return equals;
	}

	template<class T>
	static const bool AssertLessThan(const T& lhs, const T& rhs, bool including, bool throwException = false)
	{
		bool isLessThan;

		if (including)	isLessThan = (lhs <= rhs);
		else			isLessThan = (lhs <  rhs);

		if (throwException && !isLessThan)
		{
			throw std::invalid_argument(lhs + " ! less than " + rhs);
		}
		
		return isLessThan;
	}

	template<class T>
	static const bool AssertGreaterThan(const T& lhs, const T& rhs, bool including, bool throwException = false)
	{
		bool isGreaterThan;

		if (including)	isGreaterThan = (lhs >= rhs);
		else			isGreaterThan = (lhs > rhs);

		if (throwException && !isGreaterThan) 
		{
			throw std::invalid_argument(lhs + " ! greater than " + rhs);
		}

		return isGreaterThan;
	}

	static const int ToPowerOfTwo(int x)
	{
		x--;

		x |= x >> 1;
		x |= x >> 2;
		x |= x >> 4;
		x |= x >> 8;
		x |= x >> 16;

		x++;

		return x;
	}
};

