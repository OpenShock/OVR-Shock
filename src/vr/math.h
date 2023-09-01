#pragma once

#include <glm/mat4x4.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

namespace ShockLink::VR::Math {
    constexpr glm::mat4 CreateGlmMat(const glm::vec3& position);
    glm::mat4 CreateGlmMat(const glm::vec3& position, const glm::quat& rotation);

    glm::vec3 GetRotationEuler(const glm::mat4& matrix);

    constexpr bool IntersectRayPlane(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, const glm::vec3& planeOrigin, const glm::vec3& planeNormal, glm::vec3& intersect3D, glm::vec2& intersectUV, float& intersectDistance);
}