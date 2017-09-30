#include "Cube.h"

enum Normals
{
	LEFT,
	RIGHT,
	FRONT,
	BACK,
	TOP,
	BOTTOM
};

Cube::Cube(Vector3 position, Vector3 dimensions)
{
	this->position = position;
	this->dimensions = dimensions;

	GenerateFaces();

	collider = BoxCollider(faces);
}

Cube::~Cube()
{
	//delete collider;
}

void Cube::GenerateFaces() 
{
	//faces[LEFT]	= Plane(LEFT_NORMAL, Vector3(position.x, position.y, 0));
	//faces[RIGHT]	= Plane(RIGHT_NORMAL, Vector3(position.x, position.y, 0)
	//	+ Vector3(dimensions.x, 0, 0));

	//faces[BACK]	= Plane(BACK_NORMAL, Vector3(position.x, position.y, 0));
	//faces[FRONT]	= Plane(FRONT_NORMAL, Vector3(position.x, position.y, 0)
	//	+ Vector3(0, 0, dimensions.z));

	//faces[BOTTOM] = Plane(BOTTOM_NORMAL, Vector3(position.x, position.y, 0));
	//faces[TOP] = Plane(TOP_NORMAL, Vector3(position.x, position.y, 0)
	//	+ Vector3(0, dimensions.y, 0));

	faces[static_cast<int>(LEFT)] = Plane(LEFT_NORMAL, Vector3(position.x, position.y, 0));
	faces[static_cast<int>(RIGHT)] = Plane(RIGHT_NORMAL, Vector3(position.x, position.y, 0)
		+ Vector3(dimensions.x, 0, 0));

	faces[static_cast<int>(BACK)] = Plane(BACK_NORMAL, Vector3(position.x, position.y, 0));
	faces[static_cast<int>(FRONT)] = Plane(FRONT_NORMAL, Vector3(position.x, position.y, 0)
		+ Vector3(0, 0, dimensions.z));

	faces[static_cast<int>(BOTTOM)] = Plane(BOTTOM_NORMAL, Vector3(position.x, position.y, 0));
	faces[static_cast<int>(TOP)] = Plane(TOP_NORMAL, Vector3(position.x, position.y, 0)
		+ Vector3(0, dimensions.y, 0));
}

const bool Cube::SphereColliding(const Vector3& spherePosition, const float& radius) const
{
	return (collider.SphereInside(spherePosition, radius) || collider.SphereIntersecting(spherePosition, radius));
}

const bool Cube::SphereColliding(const Vector4& sphereData) const
{
	const Vector3 spherePosition(sphereData.x, sphereData.y, sphereData.z);
	const float radius = sphereData.w;

	return (collider.SphereInside(spherePosition, radius) || collider.SphereIntersecting(spherePosition, radius));
}