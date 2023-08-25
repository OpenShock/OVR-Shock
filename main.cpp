#include "vr/helpers.h"

#include <iostream>

void test(const ZapMe::Transform& ray, const ZapMe::Overlay& overlay) {
    glm::vec3 intersectionPoint3D;
    glm::vec2 intersectionPoint2D;
    float intersectionDistance;
    bool intersects = ZapMe::Helpers::IntersectRayOverlay(ray, overlay, intersectionPoint3D, intersectionPoint2D, intersectionDistance);

    // Print the results
    if (intersects) {
        std::cout << "Intersects at: " << intersectionPoint3D.x << ", " << intersectionPoint3D.y << ", " << intersectionPoint3D.z << std::endl;
    }
}

int main() {
	ZapMe::Transform ray(
	    glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 1.0f)
	);
    ZapMe::Overlay overlay(
        ZapMe::Transform {
            glm::vec3(0.0f, 0.0f, 4.0f),
            glm::vec3(1.0f, 0.0f, 1.0f),
        },
        glm::vec2(200.0f, 200.0f),
        glm::vec2(-0.5f, -0.5f)
    );
    

    test(ray, overlay);

    return 0;
}