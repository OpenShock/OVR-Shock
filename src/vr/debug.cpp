#include "debug.h"

#include <openvr.h>

void ZapMe::VR::Debug::PrintMat(const glm::mat4& mat) {
    fmt::print(
        "\t{:>4.1f} {:>4.1f} {:>4.1f} {:>4.1f}\n"
        "\t{:>4.1f} {:>4.1f} {:>4.1f} {:>4.1f}\n"
        "\t{:>4.1f} {:>4.1f} {:>4.1f} {:>4.1f}\n"
        "\t{:>4.1f} {:>4.1f} {:>4.1f} {:>4.1f}\n",
        mat[0][0], mat[1][0], mat[2][0], mat[3][0],
        mat[0][1], mat[1][1], mat[2][1], mat[3][1],
        mat[0][2], mat[1][2], mat[2][2], mat[3][2],
        mat[0][3], mat[1][3], mat[2][3], mat[3][3]
    );
}

void ZapMe::VR::Debug::PrintMat(const vr::HmdMatrix34_t& mat) {
    fmt::print(
        "\t{:>4.1f} {:>4.1f} {:>4.1f} {:>4.1f}\n"
        "\t{:>4.1f} {:>4.1f} {:>4.1f} {:>4.1f}\n"
        "\t{:>4.1f} {:>4.1f} {:>4.1f} {:>4.1f}\n",
        mat.m[0][0], mat.m[0][1], mat.m[0][2], mat.m[0][3],
        mat.m[1][0], mat.m[1][1], mat.m[1][2], mat.m[1][3],
        mat.m[2][0], mat.m[2][1], mat.m[2][2], mat.m[2][3]
    );
}

void ZapMe::VR::Debug::PrintQuat(const glm::quat& quat) {
    fmt::print(
        "\t{:>4.1f} {:>4.1f} {:>4.1f} {:>4.1f}\n",
        quat.x, quat.y, quat.z, quat.w
    );
}

void ZapMe::VR::Debug::PrintVec(const glm::vec4& vec) {
    fmt::print(
		"\t{:>4.1f} {:>4.1f} {:>4.1f} {:>4.1f}\n",
		vec.x, vec.y, vec.z, vec.w
	);
}

void ZapMe::VR::Debug::PrintVec(const glm::vec3& vec) {
    fmt::print(
        "\t{:>4.1f} {:>4.1f} {:>4.1f}\n",
        vec.x, vec.y, vec.z
    );
}