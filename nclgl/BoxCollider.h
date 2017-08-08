#pragma once

#include "Plane.h"

class BoxCollider
{
public:
	BoxCollider(Plane faces[6]);
	BoxCollider() {}
	~BoxCollider();

	const bool SphereInside(const Vector3& spherePosition, const float& radius) const;
	const bool SphereIntersecting(const Vector3& spherePosition, const float& radius) const;

private:
	Plane faces[6];

	enum class Faces;
};

