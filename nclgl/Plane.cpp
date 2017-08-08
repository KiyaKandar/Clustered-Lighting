#include "Plane.h"

Plane::Plane(const Vector3 &normal, float distance, bool normalise, Vector3 pos)
{
	if (normalise) {
		float length	= sqrt(Vector3::Dot(normal, normal));

		this->normal	= normal / length;
		this->distance	= distance / length;
	}
	else {
		this->normal	= normal;
		this->distance	= distance;
	}

	position = pos;
}

Plane::Plane(const Vector3 &normal, Vector3 pos, bool normalise)
{
	if (normalise) {
		float length = sqrt(Vector3::Dot(normal, normal));

		this->normal = normal / length;
		this->distance = pos.Length() / length;
	}
	else {
		this->normal = normal;
		this->distance = pos.Length();
	}

	position = pos;
}

bool Plane::SphereInPlane(const Vector3 &position, float radius) const {
	if (Vector3::Dot(position, normal) + distance <= -radius) {
		return false;
	}
	return true;

	//return -(Vector3::Dot(normal, position) + distance) > radius;
}

bool Plane::SphereOutsidePlane(const Vector3 &position, float radius) const {
	if (-(Vector3::Dot(position, normal) + distance) <= -radius) {
		return false;
	}
	return true;

	//return (Vector3::Dot(normal, position) + distance) > radius;
}

bool Plane::SphereIntersecting(const Vector3 &position, float radius) const {
	return (std::fabs(Vector3::Dot(position, normal) + distance)) <= radius;
}

