#include "vr/math.h"

#include <glm/geometric.hpp>
#include <glm/gtx/euler_angles.hpp>

constexpr glm::mat4 ZapMe::VR::Math::CreateGlmMat(const glm::vec3& position) {
    return glm::mat4(
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        position.x, position.y, position.z, 1.0f
    );
}

glm::mat4 ZapMe::VR::Math::CreateGlmMat(const glm::vec3& position, const glm::quat& rotation) {
    return ZapMe::VR::Math::CreateGlmMat(position) * glm::mat4_cast(rotation);
}

glm::vec3 ZapMe::VR::Math::GetRotationEuler(const glm::mat4& matrix) {
    return glm::eulerAngles(glm::quat_cast(matrix));
}

constexpr bool ZapMe::VR::Math::IntersectRayPlane(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, const glm::vec3& planeOrigin, const glm::vec3& planeNormal, glm::vec3& intersect3D, glm::vec2& intersectUV, float& intersectDistance) {
    // Calculate the dot product of the ray direction and the plane normal
    float denom = glm::dot(rayDirection, planeNormal);

    // Check if the ray and plane are not parallel
    if (glm::abs(denom) <= std::numeric_limits<float>::epsilon()) {
        return false;
    }

    // Calculate the distance from the ray's origin to the plane along the ray direction
    intersectDistance = glm::dot(planeOrigin - rayOrigin, planeNormal) / denom;

    // Check if the intersection point is in the direction of the ray
    if (intersectDistance < 0) {
        return false;
    }

    // Calculate the intersection point in 3D space
    intersect3D = rayOrigin + (rayDirection * intersectDistance);

    // Calculate the intersection point's coordinates in the plane's local space (xy plane)
    const glm::vec3 intersectionLocal = intersect3D - planeOrigin;
    intersectUV.x = glm::dot(intersectionLocal, glm::vec3(1.0f, 0.0f, 0.0f));
    intersectUV.y = glm::dot(intersectionLocal, glm::vec3(0.0f, 1.0f, 0.0f));

    return true;
}