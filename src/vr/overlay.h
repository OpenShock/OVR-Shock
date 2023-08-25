#pragma once

#include <glm/vec2.hpp>

#include "transform.h"

namespace ZapMe {

struct Overlay
{
	Overlay() : transform(), uvSize(0.0f), uvOriginOffset(-0.5f, -0.5) {}
	Overlay(const Transform& transform, const glm::vec2& uvSize, const glm::vec2& uvOriginOffset) : transform(transform), uvSize(uvSize), uvOriginOffset(uvOriginOffset) {}

	Transform transform;

	// The size of the overlay, 
	glm::vec2 uvSize;
	glm::vec2 uvOriginOffset;
};

}
