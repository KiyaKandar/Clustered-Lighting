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
	if (!faces[static_cast<int>(LEFT)].SphereInPlane(spherePosition, radius))		return false;
	if (!faces[static_cast<int>(RIGHT)].SphereInPlane(spherePosition, radius))	return false;
	if (!faces[static_cast<int>(FRONT)].SphereInPlane(spherePosition, radius))	return false;
	if (!faces[static_cast<int>(BACK)].SphereInPlane(spherePosition, radius))		return false;
	if (!faces[static_cast<int>(TOP)].SphereInPlane(spherePosition, radius))		return false;
	if (!faces[static_cast<int>(BOTTOM)].SphereInPlane(spherePosition, radius))	return false;

	return true;
}

const bool BoxCollider::SphereIntersecting(const Vector3& spherePosition, const float& radius) const
{
	float distSqr = radius * radius;

	if (spherePosition.x < faces[static_cast<int>(LEFT)].position.x)
	{
		distSqr -= std::powf(spherePosition.x - faces[static_cast<int>(LEFT)].position.x, 2);
	}
	else if (spherePosition.x > faces[static_cast<int>(RIGHT)].position.x)
	{
		distSqr -= std::powf(spherePosition.x - faces[static_cast<int>(RIGHT)].position.x, 2);
	}

	if (spherePosition.y < faces[static_cast<int>(BOTTOM)].position.y)
	{
		distSqr -= std::powf(spherePosition.y - faces[static_cast<int>(BOTTOM)].position.y, 2);
	}
	else if (spherePosition.y > faces[static_cast<int>(TOP)].position.y)
	{
		distSqr -= std::powf(spherePosition.y - faces[static_cast<int>(TOP)].position.y, 2);
	}

	if (spherePosition.z < faces[static_cast<int>(FRONT)].position.z)
	{
		distSqr -= std::powf(spherePosition.z - faces[static_cast<int>(FRONT)].position.z, 2);
	}
	else if (spherePosition.z > faces[static_cast<int>(BACK)].position.z)
	{
		distSqr -= std::powf(spherePosition.z - faces[static_cast<int>(BACK)].position.z, 2);
	}

	return distSqr > 0;
}
