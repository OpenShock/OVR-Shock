#include "tracked_device.h"

#include <array>

std::array<vr::TrackedDevicePose_t, vr::k_unMaxTrackedDeviceCount> s_trackedDevicePoses;

vr::TrackedDeviceClass ShockLink::VR::TrackedDevice::GetClass(ShockLink::VR::TrackedDevice::DeviceType deviceType) {
	switch (deviceType) {
	case ShockLink::VR::TrackedDevice::DeviceType::HMD:
		return vr::TrackedDeviceClass_HMD;
	case ShockLink::VR::TrackedDevice::DeviceType::ControllerAny:
		return vr::TrackedDeviceClass_Invalid;
	case ShockLink::VR::TrackedDevice::DeviceType::ControllerLeft:
	case ShockLink::VR::TrackedDevice::DeviceType::ControllerRight:
		return vr::TrackedDeviceClass_Controller;
	case ShockLink::VR::TrackedDevice::DeviceType::Tracker:
		return vr::TrackedDeviceClass_GenericTracker;
	case ShockLink::VR::TrackedDevice::DeviceType::TrackingReference:
		return vr::TrackedDeviceClass_TrackingReference;
	default:
		return vr::TrackedDeviceClass_Invalid;
	}
}

vr::TrackedDeviceIndex_t ShockLink::VR::TrackedDevice::GetIndex(ShockLink::VR::TrackedDevice::DeviceType deviceType) {
	vr::IVRSystem* vrSystem = vr::VRSystem();
	if (vrSystem == nullptr) {
		return vr::k_unTrackedDeviceIndexInvalid;
	}

	vr::TrackedDeviceClass deviceClass = GetClass(deviceType);

	static std::array<vr::TrackedDeviceIndex_t, vr::k_unMaxTrackedDeviceCount> devices;
	std::uint32_t nDevices = vrSystem->GetSortedTrackedDeviceIndicesOfClass(deviceClass, devices.data(), devices.size());
	if (nDevices == 0) {
		return vr::k_unTrackedDeviceIndexInvalid;
	}

	vr::ETrackedControllerRole controllerRole = GetControllerRole(deviceType);
	if (controllerRole == vr::TrackedControllerRole_Invalid) {
		// Get the first element if we are not looking for controllers
		return devices[0];
	}

	// Verify that the controller is the correct orientation
	for (std::uint32_t i = 0; i < nDevices; i++) {
		vr::TrackedDeviceIndex_t deviceIndex = devices[i];
		vr::ETrackedControllerRole role = vrSystem->GetControllerRoleForTrackedDeviceIndex(deviceIndex);

		if (role == controllerRole || ((role == vr::TrackedControllerRole_LeftHand || role == vr::TrackedControllerRole_RightHand || role == vr::TrackedControllerRole_OptOut) && controllerRole == vr::TrackedControllerRole_OptOut)) {
			return deviceIndex;
		}
	}

	return vr::k_unTrackedDeviceIndexInvalid;
}

vr::ETrackedControllerRole ShockLink::VR::TrackedDevice::GetControllerRole(ShockLink::VR::TrackedDevice::DeviceType deviceType) {
	switch (deviceType) {
	case ShockLink::VR::TrackedDevice::DeviceType::ControllerAny:
		return vr::TrackedControllerRole_OptOut;
	case ShockLink::VR::TrackedDevice::DeviceType::ControllerLeft:
		return vr::TrackedControllerRole_LeftHand;
	case ShockLink::VR::TrackedDevice::DeviceType::ControllerRight:
		return vr::TrackedControllerRole_RightHand;
	default:
		return vr::TrackedControllerRole_Invalid;
	}
}