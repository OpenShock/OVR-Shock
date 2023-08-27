#include "vr/transform.h"

#include <glm/gtx/transform.hpp>
#include <glm/gtx/euler_angles.hpp>

#include <fmt/core.h>

ZapMe::VR::Transform::Transform()
	: pos(0.0f)
	, rot(0.0f)
{}

ZapMe::VR::Transform::Transform(const glm::mat4& matrix)
	: pos(matrix[3])
	, rot(glm::eulerAngles(glm::quat_cast(matrix)))
{}

ZapMe::VR::Transform::Transform(const vr::HmdMatrix34_t& matrix)
	: pos(matrix.m[0][3], matrix.m[1][3], matrix.m[2][3])
	, rot(glm::eulerAngles(glm::quat_cast(glm::mat4(
		matrix.m[0][0], matrix.m[0][1], matrix.m[0][2], 0.0f,
		matrix.m[1][0], matrix.m[1][1], matrix.m[1][2], 0.0f,
		matrix.m[2][0], matrix.m[2][1], matrix.m[2][2], 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	))))
{}

ZapMe::VR::Transform::Transform(const glm::vec3& position, const glm::vec3& rotation)
	: pos(position)
	, rot(rotation)
{}

glm::mat4x4 ZapMe::VR::Transform::ToMatrix() const
{
	return glm::translate(pos) * glm::eulerAngleXYZ(rot.x, rot.y, rot.z);
}

vr::HmdMatrix34_t ZapMe::VR::Transform::ToHmdMatrix34() const
{
	const glm::mat4 matrix = ToMatrix();

	fmt::print("Matrix:\n");
	fmt::print("{}  {}  {}  {}\n", matrix[0][0], matrix[0][1], matrix[0][2], matrix[0][3]);
	fmt::print("{}  {}  {}  {}\n", matrix[1][0], matrix[1][1], matrix[1][2], matrix[1][3]);
	fmt::print("{}  {}  {}  {}\n", matrix[2][0], matrix[2][1], matrix[2][2], matrix[2][3]);
	fmt::print("{}  {}  {}  {}\n", matrix[3][0], matrix[3][1], matrix[3][2], matrix[3][3]);
	fmt::print("\n");

	vr::HmdMatrix34_t result = { 0 };
	for (int i = 0; i < 3; ++i)
	{
		result.m[i][0] = matrix[0][i];
		result.m[i][1] = matrix[1][i];
		result.m[i][2] = matrix[2][i];
		result.m[i][3] = matrix[3][i];
	}

	fmt::print("HmdMatrix34_t:\n");
	fmt::print("{}  {}  {}  {}\n", result.m[0][0], result.m[0][1], result.m[0][2], result.m[0][3]);
	fmt::print("{}  {}  {}  {}\n", result.m[1][0], result.m[1][1], result.m[1][2], result.m[1][3]);
	fmt::print("{}  {}  {}  {}\n", result.m[2][0], result.m[2][1], result.m[2][2], result.m[2][3]);
	fmt::print("\n");

	return result;
}