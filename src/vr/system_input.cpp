#include "vr/system.h"

#include "vr/overlay.h"
#include "vr/tracked_device.h"

#include <openvr.h>
#include <fmt/format.h>
#include <vector>
#include <array>

static std::vector<ZapMe::VR::Overlay*> s_overlays;

void ZapMe::VR::VRSystem::RegisterOverlay(ZapMe::VR::Overlay* overlay)
{
	s_overlays.push_back(overlay);
}

void ZapMe::VR::VRSystem::UnregisterOverlay(ZapMe::VR::Overlay* overlay)
{
	s_overlays.erase(std::remove(s_overlays.begin(), s_overlays.end(), overlay), s_overlays.end());
}

static std::array<vr::TrackedDevicePose_t, vr::k_unMaxTrackedDeviceCount> s_devices;
static std::array<vr::TrackedDevicePose_t, vr::k_unMaxTrackedDeviceCount> s_prevDevices;

void OnDeviceConnected(std::uint32_t index, const vr::TrackedDevicePose_t& pose)
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
}
void OnDeviceDisconnected(std::uint32_t index)
{
	fmt::print("[DEVICE #{:0>2}] Disconnected\n", index);
}
void OnDevicePoseValidated(std::uint32_t index, const vr::TrackedDevicePose_t& pose)
{
	fmt::print("[DEVICE #{:0>2}] Pose validated\n", index);
}
void OnDevicePoseInvalidated(std::uint32_t index)
{
	fmt::print("[DEVICE #{:0>2}] Pose invalidated\n", index);
}
void OnDevicePoseUpdated(std::uint32_t index, const vr::TrackedDevicePose_t& pose)
{
}
void ZapMe::VR::VRSystem::PollInput()
{
	auto system = vr::VRSystem();
	if (!system) return;

	const constexpr std::size_t NDEVICES = vr::k_unMaxTrackedDeviceCount;

	static_assert(s_devices.size() == s_prevDevices.size() && s_devices.size() == NDEVICES, "Device array size mismatch");
	std::memcpy(s_prevDevices.data(), s_devices.data(), NDEVICES * sizeof(vr::TrackedDevicePose_t));

	system->GetDeviceToAbsoluteTrackingPose(vr::TrackingUniverseStanding, 0.0f, s_devices.data(), NDEVICES);

	for (std::uint32_t i = 0; i < NDEVICES; i++)
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
				OnDevicePoseInvalidated(i);
				OnDeviceDisconnected(i);
			}
		}

		if (!pose.bDeviceIsConnected) continue;

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
