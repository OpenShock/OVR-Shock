#pragma once

#include <glm/gtc/quaternion.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <openvr.h>

namespace ShockLink::VR {
struct Transform {
	Transform();
	Transform(const glm::mat4& matrix);
	Transform(const vr::HmdMatrix34_t& matrix);
	Transform(const glm::vec3& position, const glm::quat& rotation);
	Transform(const glm::vec3& position, const glm::vec3& rotation);

	inline glm::vec3 Position() const { return pos; }
	void SetPosition(const glm::vec3& position);

	inline glm::quat Rotation() const { return rot; }
	void SetRotation(const glm::quat& rotation);

	glm::vec3 RotationEuler() const;
	void SetRotationEuler(const glm::vec3& rotation);

	// Conversion functions
	inline glm::mat4 ToGlmMatrix() const { return mat; }
	vr::HmdMatrix34_t ToHmdMatrix() const;

	// Implicit conversion operators
	inline operator glm::mat4() const { return mat; }
	inline operator vr::HmdMatrix34_t() const { return ToHmdMatrix(); }

	// Assignment operators
	Transform& operator = (const glm::mat4& other);
	Transform& operator = (const vr::HmdMatrix34_t& other);

	inline bool operator == (const glm::mat4& other) const {
		return mat == other;
	}
	inline bool operator != (const glm::mat4& other) const {
		return mat != other;
	}
	inline bool operator == (const Transform& other) const {
		return mat == other.mat;
	}
	inline bool operator != (const Transform& other) const {
		return mat != other.mat;
	}
	glm::mat4 mat;
	glm::quat rot;
	glm::vec3 pos;
};
}