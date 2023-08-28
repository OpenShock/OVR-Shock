#include "vr/transform.h"

#include "vr/math.h"
#include "vr/conversion.h"

ZapMe::VR::Transform::Transform()
	: mat()
	, rot()
	, pos(0.0f)
{}

ZapMe::VR::Transform::Transform(const glm::mat4& matrix)
	: mat(matrix)
	, rot(glm::quat_cast(matrix))
	, pos(matrix[3])
{}

ZapMe::VR::Transform::Transform(const vr::HmdMatrix34_t& matrix)
	: Transform(Conversion::ToGlmMat(matrix))
{}

ZapMe::VR::Transform::Transform(const glm::vec3& position, const glm::quat& rotation)
	: mat(Math::CreateGlmMat(position, rotation))
	, rot(rotation)
	, pos(position)
{}

ZapMe::VR::Transform::Transform(const glm::vec3& position, const glm::vec3& rotation)
	: Transform(position, glm::quat(rotation))
{}

void ZapMe::VR::Transform::SetPosition(const glm::vec3& position) {
	mat = Math::CreateGlmMat(position, rot);
	pos = position;
}

void ZapMe::VR::Transform::SetRotation(const glm::quat& rotation) {
	mat = Math::CreateGlmMat(pos, rotation);
	rot = rotation;
}

glm::vec3 ZapMe::VR::Transform::RotationEuler() const {
	return glm::eulerAngles(rot);
}

void ZapMe::VR::Transform::SetRotationEuler(const glm::vec3& rotation) {
	SetRotation(glm::quat(rotation));
}

vr::HmdMatrix34_t ZapMe::VR::Transform::ToHmdMatrix() const {
	return Conversion::ToHmdMat(mat);
}