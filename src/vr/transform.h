#pragma once

#include <openvr.h>

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

namespace ZapMe::VR {
struct Transform
{
	Transform();
	Transform(const glm::mat4& matrix);
	Transform(const vr::HmdMatrix34_t& matrix);
	Transform(const glm::vec3& position, const glm::vec3& rotation);

	glm::vec3 pos;
	glm::vec3 rot;

	// Converts the transform to a 4x4 matrix
	glm::mat4 ToMatrix() const;
	vr::HmdMatrix34_t ToHmdMatrix34() const;
};
}