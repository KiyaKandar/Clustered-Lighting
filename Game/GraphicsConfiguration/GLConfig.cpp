#include "GLConfig.h"

const float GLConfig::NEAR_PLANE = 1.0f;
const float GLConfig::FAR_PLANE = 4000.0f;

const Vector2 GLConfig::MIN_NDC_COORDS = Vector2(-1, -1);
const Vector2 GLConfig::MAX_NDC_COORDS = Vector2(1, 1);

const Vector2 GLConfig::RESOLUTION = Vector2(1280, 720);

const Matrix4 GLConfig::SHARED_PROJ_MATRIX = 
	Matrix4::Perspective(NEAR_PLANE, FAR_PLANE, RESOLUTION.x / RESOLUTION.y, 45.0f);