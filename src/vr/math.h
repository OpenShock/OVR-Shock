#pragma once

#include "vr/overlay.h"
#include "vr/transform.h"

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

namespace ZapMe::VR::Math {
    constexpr bool IntersectRayPlane(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, const glm::vec3& planeOrigin, const glm::vec3& planeNormal, glm::vec3& intersect3D, glm::vec2& intersectUV, float& intersectDistance);
}