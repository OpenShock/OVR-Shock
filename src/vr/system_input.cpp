#include "vr/system.h"

#include "vr/overlay.h"
#include "vr/tracking_reference.h"

#include <openvr.h>
#include <fmt/format.h>
#include <vector>
#include <array>

static std::vector<ShockLink::VR::Overlay*> s_overlays;
static std::vector<ShockLink::VR::TrackingReference*> s_trackingReferences;
static std::array<vr::TrackedDevicePose_t, vr::k_unMaxTrackedDeviceCount> s_devices;
static std::array<vr::TrackedDevicePose_t, vr::k_unMaxTrackedDeviceCount> s_prevDevices;

void ShockLink::VR::VRSystem::RegisterOverlay(ShockLink::VR::Overlay* overlay)
{
	s_overlays.push_back(overlay);
}

void ShockLink::VR::VRSystem::UnregisterOverlay(ShockLink::VR::Overlay* overlay)
{
	s_overlays.erase(std::remove(s_overlays.begin(), s_overlays.end(), overlay), s_overlays.end());
}

void ShockLink::VR::VRSystem::RegisterTrackingReference(ShockLink::VR::TrackingReference* reference)
{
	s_trackingReferences.push_back(reference);
}

void ShockLink::VR::VRSystem::UnregisterTrackingReference(ShockLink::VR::TrackingReference* reference)
{
	s_trackingReferences.erase(std::remove(s_trackingReferences.begin(), s_trackingReferences.end(), reference), s_trackingReferences.end());
}

void OnDeviceConnected(vr::TrackedDeviceIndex_t index, const vr::TrackedDevicePose_t& pose)
{
	auto deviceClass = vr::VRSystem()->GetTrackedDeviceClass(index);
	const char* deviceClassStr;
	switch (deviceClass)
	{
		case vr::TrackedDeviceClass_Invalid:
			fmt::print("[DEVICE #{:0>2}] Invalid device class connected\n", index);
			return;
		case vr::TrackedDeviceClass_HMD:
			fmt::print("[DEVICE #{:0>2}] HMD connected\n", index);
			break;
		case vr::TrackedDeviceClass_Controller:
			fmt::print("[DEVICE #{:0>2}] Controller connected\n", index);
			break;
		case vr::TrackedDeviceClass_GenericTracker:
			fmt::print("[DEVICE #{:0>2}] GenericTracker connected\n", index);
			break;
		case vr::TrackedDeviceClass_TrackingReference:
			fmt::print("[DEVICE #{:0>2}] TrackingReference connected\n", index);
			break;
		case vr::TrackedDeviceClass_DisplayRedirect:
			fmt::print("[DEVICE #{:0>2}] DisplayRedirect connected\n", index);
			break;
		default:
			fmt::print("[DEVICE #{:0>2}] Unknown device class connected\n", index);
			return;
	}
	for (auto reference : s_trackingReferences)
	{
		reference->OnDeviceConnected(index, deviceClass, pose);
	}
	for (auto overlay : s_overlays)
	{
		overlay->OnDeviceConnected(index, deviceClass, pose);
	}
}
void OnDeviceDisconnected(vr::TrackedDeviceIndex_t index)
{
	fmt::print("[DEVICE #{:0>2}] Disconnected\n", index);
	for (auto reference : s_trackingReferences)
	{
		reference->OnDeviceDisconnected(index);
	}
	for (auto overlay : s_overlays)
	{
		overlay->OnDeviceDisconnected(index);
	}
}
void OnDevicePoseValidated(vr::TrackedDeviceIndex_t index, const vr::TrackedDevicePose_t& pose)
{
	fmt::print("[DEVICE #{:0>2}] Pose validated\n", index);
	for (auto reference : s_trackingReferences)
	{
		reference->OnDevicePoseValidated(index, pose);
	}
	for (auto overlay : s_overlays)
	{
		overlay->OnDevicePoseValidated(index, pose);
	}
}
void OnDevicePoseInvalidated(vr::TrackedDeviceIndex_t index)
{
	fmt::print("[DEVICE #{:0>2}] Pose invalidated\n", index);
	for (auto reference : s_trackingReferences)
	{
		reference->OnDevicePoseInvalidated(index);
	}
	for (auto overlay : s_overlays)
	{
		overlay->OnDevicePoseInvalidated(index);
	}
}
void OnDevicePoseUpdated(vr::TrackedDeviceIndex_t index, const vr::TrackedDevicePose_t& pose)
{
	for (auto reference : s_trackingReferences)
	{
		reference->OnDevicePoseUpdated(index, pose);
	}
	for (auto overlay : s_overlays)
	{
		overlay->OnDevicePoseUpdated(index, pose);
	}
}

void ShockLink::VR::VRSystem::PollInput()
{
	auto system = vr::VRSystem();
	if (!system) return;

	const constexpr std::size_t NDEVICES = vr::k_unMaxTrackedDeviceCount;

	static_assert(s_devices.size() == s_prevDevices.size() && s_devices.size() == NDEVICES, "Device array size mismatch");
	std::memcpy(s_prevDevices.data(), s_devices.data(), NDEVICES * sizeof(vr::TrackedDevicePose_t));

	system->GetDeviceToAbsoluteTrackingPose(vr::TrackingUniverseStanding, 0.0f, s_devices.data(), NDEVICES);

	for (vr::TrackedDeviceIndex_t i = 0; i < NDEVICES; i++)
	{
		const auto& pose = s_devices[i];
		const auto& prevPose = s_prevDevices[i];

		if (prevPose.bDeviceIsConnected != pose.bDeviceIsConnected)
		{
			if (pose.bDeviceIsConnected)
			{
				OnDeviceConnected(i, pose);
			}
			else
			{
				OnDeviceDisconnected(i);
				continue;
			}
		}
		else if (!pose.bDeviceIsConnected)
		{
			continue;
		}

		if (prevPose.bPoseIsValid != pose.bPoseIsValid)
		{
			if (pose.bPoseIsValid)
			{
				OnDevicePoseValidated(i, pose);
			}
			else
			{
				OnDevicePoseInvalidated(i);
				continue;
			}
		}

		OnDevicePoseUpdated(i, pose);
	}
}
