#pragma once

#include <glm/mat4x4.hpp>
#include <openvr.h>

namespace ZapMe::VR::Conversion {
	constexpr glm::mat4 ToGlmMat(const vr::HmdMatrix34_t& matrix) {
		return glm::mat4(
			matrix.m[0][0], matrix.m[1][0], matrix.m[2][0], 0.0f,
			matrix.m[0][1], matrix.m[1][1], matrix.m[2][1], 0.0f,
			matrix.m[0][2], matrix.m[1][2], matrix.m[2][2], 0.0f,
			matrix.m[0][3], matrix.m[1][3], matrix.m[2][3], 1.0f
		);
	}
	constexpr vr::HmdMatrix34_t ToHmdMat(const glm::mat4& matrix) {
		return vr::HmdMatrix34_t{
			matrix[0][0], matrix[1][0], matrix[2][0], matrix[3][0],
			matrix[0][1], matrix[1][1], matrix[2][1], matrix[3][1],
			matrix[0][2], matrix[1][2], matrix[2][2], matrix[3][2]
		};
	}
}