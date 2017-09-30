#pragma once

#include "Plane.h"
#include "Vector3.h"
#include "Vector4.h"
#include "BoxCollider.h"

const int NUM_FACES = 6;

const Vector3 LEFT_NORMAL = Vector3(-1, 0, 0);
const Vector3 FRONT_NORMAL = Vector3(0, 0, -1);
const Vector3 RIGHT_NORMAL = Vector3(1, 0, 0);
const Vector3 BACK_NORMAL = Vector3(0, 0, 1);
const Vector3 TOP_NORMAL = Vector3(0, 1, 0);
const Vector3 BOTTOM_NORMAL = Vector3(0, -1, 0);

/*
  Position coordinate is bottom-left-near of cube.
*/
class Cube
{
public:
	Cube(Vector3 position = Vector3(), Vector3 dimensions = Vector3());
	~Cube();

	const Plane* GetFaces() const
	{
		return faces;
	}

	const bool SphereColliding(const Vector3& spherePosition, const float& radius) const;
	const bool SphereColliding(const Vector4& sphereData) const;

private:
	void GenerateFaces();

	Vector3 position;
	Vector3 dimensions;

	Plane faces[NUM_FACES];
	BoxCollider collider;

	enum class Indexes;
};

