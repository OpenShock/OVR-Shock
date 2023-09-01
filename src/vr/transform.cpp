#include "vr/transform.h"

#include "vr/math.h"
#include "vr/conversion.h"

ShockLink::VR::Transform::Transform()
	: mat()
	, rot()
	, pos(0.0f)
{}

ShockLink::VR::Transform::Transform(const glm::mat4& matrix)
	: mat(matrix)
	, rot(glm::quat_cast(matrix))
	, pos(matrix[3])
{}

ShockLink::VR::Transform::Transform(const vr::HmdMatrix34_t& matrix)
	: Transform(Conversion::ToGlmMat(matrix))
{}

ShockLink::VR::Transform::Transform(const glm::vec3& position, const glm::quat& rotation)
	: mat(Math::CreateGlmMat(position, rotation))
	, rot(rotation)
	, pos(position)
{}

ShockLink::VR::Transform::Transform(const glm::vec3& position, const glm::vec3& rotation)
	: Transform(position, glm::quat(rotation))
{}

void ShockLink::VR::Transform::SetPosition(const glm::vec3& position) {
	mat = Math::CreateGlmMat(position, rot);
	pos = position;
}

void ShockLink::VR::Transform::SetRotation(const glm::quat& rotation) {
	mat = Math::CreateGlmMat(pos, rotation);
	rot = rotation;
}

glm::vec3 ShockLink::VR::Transform::RotationEuler() const {
	return glm::eulerAngles(rot);
}

void ShockLink::VR::Transform::SetRotationEuler(const glm::vec3& rotation) {
	SetRotation(glm::quat(rotation));
}

vr::HmdMatrix34_t ShockLink::VR::Transform::ToHmdMatrix() const {
	return Conversion::ToHmdMat(mat);
}