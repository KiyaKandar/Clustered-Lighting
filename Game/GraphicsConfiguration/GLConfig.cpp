#include "GLConfig.h"

const Vector2 GLConfig::MIN_NDC_COORDS = Vector2(-1, -1);
const Vector2 GLConfig::MAX_NDC_COORDS = Vector2(1, 1);

const Vector2 GLConfig::RESOLUTION = Vector2(1920, 1080);

const Matrix4 GLConfig::SHARED_PROJ_MATRIX = 
	Matrix4::Perspective(1.0f, 15000.0f, RESOLUTION.x / RESOLUTION.y, 60.0f);