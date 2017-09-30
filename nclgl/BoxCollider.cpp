#include "BoxCollider.h"

enum Faces
{
	LEFT,
	RIGHT,
	FRONT,
	BACK,
	TOP,
	BOTTOM,
};

BoxCollider::BoxCollider(Plane faces[6])
{
	memcpy(this->faces, faces, 6 * sizeof(Plane));
}

BoxCollider::~BoxCollider()
{
}

const bool BoxCollider::SphereInside(const Vector3& spherePosition, const float& radius) const
{
	if (!faces[LEFT].SphereInPlane(spherePosition, radius))		return false;
	if (!faces[RIGHT].SphereInPlane(spherePosition, radius))	return false;
	if (!faces[FRONT].SphereInPlane(spherePosition, radius))	return false;
	if (!faces[BACK].SphereInPlane(spherePosition, radius))		return false;
	if (!faces[TOP].SphereInPlane(spherePosition, radius))		return false;
	if (!faces[BOTTOM].SphereInPlane(spherePosition, radius))	return false;

	return true;
}

const bool BoxCollider::SphereIntersecting(const Vector3& spherePosition, const float& radius) const
{
	float distSqr = radius * radius;

	if (spherePosition.x < faces[LEFT].position.x)
	{
		distSqr -= std::powf(spherePosition.x - faces[LEFT].position.x, 2);
	}
	else if (spherePosition.x > faces[RIGHT].position.x)
	{
		distSqr -= std::powf(spherePosition.x - faces[RIGHT].position.x, 2);
	}

	if (spherePosition.y < faces[BOTTOM].position.y)
	{
		distSqr -= std::powf(spherePosition.y - faces[BOTTOM].position.y, 2);
	}
	else if (spherePosition.y > faces[TOP].position.y)
	{
		distSqr -= std::powf(spherePosition.y - faces[TOP].position.y, 2);
	}

	if (spherePosition.z < faces[FRONT].position.z)
	{
		distSqr -= std::powf(spherePosition.z - faces[FRONT].position.z, 2);
	}
	else if (spherePosition.z > faces[BACK].position.z)
	{
		distSqr -= std::powf(spherePosition.z - faces[BACK].position.z, 2);
	}

	return distSqr > 0;
}
