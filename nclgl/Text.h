#pragma once

#include <string>
#include "Vector3.h"

class Text
{
public:

	Text(std::string text = "", Vector3 position = Vector3(), 
		float size = 0, bool perspective = false)
		:text(text),
		position(position),
		size(size),
		perspective(perspective) {}

	~Text() {}

	std::string text;
	Vector3 position;
	float size;
	bool perspective;
};

