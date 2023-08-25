#pragma once

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

namespace ZapMe {

struct Transform
{
	Transform() : pos(0.0f), rot(0.0f) {}
	Transform(const glm::mat4& matrix);
	Transform(const glm::vec3& position, const glm::vec3& rotation) : pos(position), rot(rotation) {}

	glm::vec3 pos;
	glm::vec3 rot;

	// Converts the transform to a 4x4 matrix
	glm::mat4 ToMatrix() const;
};

}