#pragma once

#include "../nclgl/Maths/Vector2.h"
#include "../nclgl/Maths/Matrix4.h"

class GLConfig
{
public:

	enum GBUFFER
	{
		GPOSITION,
		GNORMAL,
		GALBEDO
	};

	enum AMBIENT_TEX
	{
		SSAO_INDEX
	};
	
	static const int BLOOM_STRENGTH = 2;

	static const int NUM_X_AXIS_TILES = 10;
	static const int NUM_Y_AXIS_TILES = 10;
	static const int NUM_Z_AXIS_TILES = 10;

	static const int SHADOW_LIGHTS = 5;

	//Declared in cpp file - volatile integral type problem.
	static const Vector2 MIN_NDC_COORDS;
	static const Vector2 MAX_NDC_COORDS;
	static const Vector2 RESOLUTION;

	static const Matrix4 SHARED_PROJ_MATRIX;
};

