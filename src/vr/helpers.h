#pragma once

#include "vr/overlay.h"
#include "vr/transform.h"

#include <glm/geometric.hpp>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

namespace ZapMe::VR::Helpers {
    constexpr bool IntersectRayPlane(
        const glm::vec3& rayOrigin,
        const glm::vec3& rayDirection,
        const glm::vec3& planeOrigin,
        const glm::vec3& planeNormal,
        glm::vec3& intersect3D,
        glm::vec2& intersectUV,
        float& intersectDistance
    ) {
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
    constexpr bool IntersectRayOverlay(
		const Transform& ray,
        const Overlay& overlay,
        glm::vec3& intersect3D,
        glm::vec2& intersectUV,
        float& intersectDistance
    ) {
        if (!IntersectRayPlane(ray.pos, ray.rot, overlay.m_transform.pos, overlay.m_transform.rot, intersect3D, intersectUV, intersectDistance)) {
			return false;
		}

        intersectUV += overlay.m_uvOriginOffset;

        return intersectUV.x < 0.0f
    	    || intersectUV.y < 0.0f
    	    || intersectUV.x > overlay.m_uvSize.x
    	    || intersectUV.y > overlay.m_uvSize.y;
    }
}