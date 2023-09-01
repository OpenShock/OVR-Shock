#include "vr/system.h"

#include <fmt/core.h>
#include <openvr.h>

vr::IVRSystem* s_vrSystem;

bool ShockLink::VR::VRSystem::Initialize() {
	vr::EVRInitError error;
	s_vrSystem = vr::VR_Init(&error, vr::VRApplication_Overlay);
	if (error != vr::VRInitError_None) {
		fmt::print("Failed to initialize OpenVR: {}\n", vr::VR_GetVRInitErrorAsEnglishDescription(error));
		return false;
	}

	return true;
}

void ShockLink::VR::VRSystem::Shutdown() {
	s_vrSystem = nullptr;
	vr::VR_Shutdown();
}

bool ShockLink::VR::VRSystem::IsInitialized() {
	return s_vrSystem != nullptr;
}