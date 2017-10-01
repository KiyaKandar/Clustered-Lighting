#pragma once
/*
	AUTHOR: RICH DAVISON
*/


#include "Vector3.h"

class Plane
{
public:
	Plane(void) {};
	Plane(const Vector3 &normal, float distance, bool normalise = false, Vector3 pos = Vector3());
	Plane(const Vector3 &normal, Vector3 pos, bool normalise = false);
	~Plane(void) {};

	void	SetNormal(const Vector3 &normal)	{ this->normal = normal; }
	Vector3 GetNormal()					const	{ return normal; }

	void	SetDistance(float dist)				{ distance = dist; }
	float	GetDistance()				const	{ return distance; }

	bool SphereInPlane(const Vector3 &position, float radius)		const;
	bool SphereOutsidePlane(const Vector3& position, float radius)	const;
	bool SphereIntersecting(const Vector3& position, float radius)	const;

	Vector3 position;
protected:
	Vector3 normal;
	float distance;

};

