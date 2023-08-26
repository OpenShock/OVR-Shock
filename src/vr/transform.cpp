#include "vr/transform.h"

#include <glm/gtx/transform.hpp>
#include <glm/gtx/euler_angles.hpp>

ZapMe::VR::Transform::Transform(const glm::mat4& matrix) : pos(matrix[3]), rot(glm::eulerAngles(glm::quat_cast(matrix))) {}

glm::mat4x4 ZapMe::VR::Transform::ToMatrix() const
{
	return glm::translate(pos) * glm::eulerAngleXYZ(rot.x, rot.y, rot.z);


}