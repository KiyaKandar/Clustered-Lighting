#pragma once
/*
	AUTHOR: RICH DAVISON
*/

#include "Plane.h"
#include "Matrix4.h"
#include "SceneNode.h"
#include "ModelMesh.h"

class Matrix4; // Compile Mat4 class first

class Frustum
{
public:
	Frustum(void) {};
	~Frustum(void) {};

	void FromMatrix(const Matrix4 &mvp);
	bool InsideFrustum(const BoundingBox &b);

protected:
	Plane planes[6];
};

