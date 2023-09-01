#pragma once

#include <glm/mat4x4.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/vec4.hpp>
#include <glm/vec3.hpp>
#include <fmt/format.h>

namespace vr { struct HmdMatrix34_t; }

namespace ShockLink::VR::Debug {
	void PrintMat(const glm::mat4& mat);
	void PrintMat(const vr::HmdMatrix34_t& mat);
	void PrintQuat(const glm::quat& quat);
	void PrintVec(const glm::vec4& vec);
	void PrintVec(const glm::vec3& vec);
}
