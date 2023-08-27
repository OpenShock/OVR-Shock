#include "vr/system.h"

#include <fmt/core.h>
#include <openvr.h>

vr::IVRSystem* s_vrSystem;

bool ZapMe::VR::VRSystem::Initialize() {
	vr::EVRInitError error;
	s_vrSystem = vr::VR_Init(&error, vr::VRApplication_Overlay);
	if (error != vr::VRInitError_None) {
		fmt::print("Failed to initialize OpenVR: {}\n", vr::VR_GetVRInitErrorAsEnglishDescription(error));
		return false;
	}

	return true;
}

void ZapMe::VR::VRSystem::Shutdown() {
	s_vrSystem = nullptr;
	vr::VR_Shutdown();
}

bool ZapMe::VR::VRSystem::IsInitialized() {
	return s_vrSystem != nullptr;
}