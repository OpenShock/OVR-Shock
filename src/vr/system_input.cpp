#include "vr/system.h"

#include "vr/overlay.h"

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
	fmt::print("Device {} connected\n", index);
}
void OnDeviceDisconnected(std::uint32_t index)
{
	fmt::print("Device {} disconnected\n", index);
}
void OnDevicePoseValidated(std::uint32_t index, const vr::TrackedDevicePose_t& pose)
{
	fmt::print("Device {} pose validated\n", index);
}
void OnDevicePoseInvalidated(std::uint32_t index)
{
	fmt::print("Device {} pose invalidated\n", index);
}
void OnDevicePoseUpdated(std::uint32_t index, const vr::TrackedDevicePose_t& pose)
{
	fmt::print("Device {} pose updated\n", index);
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
				OnDeviceDisconnected(i);
				continue;
			}
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
