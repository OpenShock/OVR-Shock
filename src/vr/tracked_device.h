#pragma once

#include <openvr.h>
#include <cstdint>

namespace ShockLink::VR {
struct TrackedDevice
{
	enum class DeviceType : std::uint8_t
	{
		HMD,
		ControllerAny,
		ControllerLeft,
		ControllerRight,
		Tracker,
		TrackingReference,
		Undefined
	};
	static vr::TrackedDeviceClass GetClass(DeviceType type);
	static vr::TrackedDeviceIndex_t GetIndex(DeviceType type);
	static vr::ETrackedControllerRole GetControllerRole(DeviceType type);
};
}