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
		GALBEDO,
		GMETALLIC,
		GROUGHNESS
	};

	enum AMBIENT_TEX
	{
		SSAO_INDEX
	};
	
	static const int BLOOM_STRENGTH = 2;

	static const int NUM_X_AXIS_TILES = 16;
	static const int NUM_Y_AXIS_TILES = 16;
	static const int NUM_Z_AXIS_TILES = 16;
	static constexpr int NUM_TILES = NUM_X_AXIS_TILES * NUM_Y_AXIS_TILES * NUM_Z_AXIS_TILES;

	static const int NUM_LIGHTS = 1000;
	static const int SHADOW_LIGHTS = 2;

	//Declared in cpp file - volatile integral type problem.
	static const Vector2 MIN_NDC_COORDS;
	static const Vector2 MAX_NDC_COORDS;
	static const Vector2 RESOLUTION;

	static const float NEAR_PLANE;
	static const float FAR_PLANE;
	static const Matrix4 SHARED_PROJ_MATRIX;
};

