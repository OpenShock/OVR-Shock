#include <iostream>
#include <glm/glm.hpp>

struct Ray {
	glm::vec3 origin;
	glm::vec3 direction;
};
struct Plane {
	glm::vec3 origin;
	glm::vec3 normal;
	glm::vec2 size;
};
struct PlaneIntersection {
	glm::vec2 uvPoint;
	float distance;
};

// Gets the intersection between a ray and a plane, the plane is along the xy plane of the plane transform
bool IntersectPlane(const Ray& ray, const Plane& plane, PlaneIntersection& intersection) {
    // Calculate the dot product of the ray direction and the plane normal
    float denom = glm::dot(ray.direction, plane.normal);

    // Check if the ray and plane are not parallel
    if (glm::abs(denom) <= std::numeric_limits<float>::epsilon()) {
        std::cout << "Ray and plane are parallel" << std::endl;
        return false; // No intersection
    }

    // Calculate the distance from the ray's origin to the plane along the ray direction
    intersection.distance = glm::dot(plane.origin - ray.origin, plane.normal) / denom;

    // Check if the intersection point is in the direction of the ray
    if (intersection.distance < 0) {
        std::cout << "Intersection point is behind the ray" << std::endl;
        return false; // No intersection
    }

    // Calculate the intersection point in 3D space
    glm::vec3 intersection3D = ray.origin + (ray.direction * intersection.distance);

    // Calculate the intersection point's coordinates in the plane's local space (xy plane)
    glm::vec3 intersectionLocal = intersection3D - plane.origin;
    intersection.uvPoint.x = glm::dot(intersectionLocal, glm::vec3(1.0f, 0.0f, 0.0f));
    intersection.uvPoint.y = glm::dot(intersectionLocal, glm::vec3(0.0f, 1.0f, 0.0f));

    // Check if the intersection point is within the plane's bounds
    // TODO: Figure out OpenVR's plane origin point, the following code is for a plane with the origin at the center
    if (glm::abs(intersection.uvPoint.x) > plane.size.x || glm::abs(intersection.uvPoint.y) > plane.size.y) {
        std::cout << "Intersection point is outside the plane's bounds [" << plane.size.x << ", " << plane.size.y << "] < " << intersection.uvPoint.x << ", " << intersection.uvPoint.y << ">" << std::endl;
        return false; // No intersection
    }

    return true; // Intersection occurred
}

void test(const Ray& ray, const Plane& plane) {
    PlaneIntersection intersection;
    bool intersects = IntersectPlane(ray, plane, intersection);

    // Print the results
    if (intersects) {
        std::cout << "Intersection occurred at point (" << intersection.uvPoint.x << ", " << intersection.uvPoint.y << ") at a distance of " << intersection.distance << std::endl;
    }
}

int main() {
    Ray ray{
	    glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 1.0f)
	};
    Plane plane{
        glm::vec3(0.0f, 0.0f, 4.0f),
        glm::vec3(1.0f, 0.0f, 1.0f),
        glm::vec2(200.0f, 200.0f)
    };

    test(ray, plane);

    return 0;
}