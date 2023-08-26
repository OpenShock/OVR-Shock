#include "vr/vr_manager.h"

#include "vr/overlay.h"

#include <fmt/core.h>
#include <openvr.h>
#include <array>
#include <bitset>
#include <vector>

void PollDevicePoses();

vr::IVRSystem* s_vrSystem;
vr::IVROverlay* s_vrOverlay;
std::array<vr::TrackedDevicePose_t, vr::k_unMaxTrackedDeviceCount> s_devices;
std::vector<std::shared_ptr<ZapMe::VR::Overlay>> s_overlays;

bool ZapMe::VR::VRManager::Initialize()
{
	if (s_vrSystem) return true;

	s_overlays.clear();

	vr::EVRInitError error;
	s_vrSystem = vr::VR_Init(&error, vr::VRApplication_Overlay);
	if (error != vr::VRInitError_None) {
		s_vrSystem = nullptr;
		return false;
	}

	s_vrOverlay = vr::VROverlay();

	return true;
}
void ZapMe::VR::VRManager::Shutdown()
{
	s_vrSystem = nullptr;
	s_vrOverlay = nullptr;
	s_overlays.clear();

	if (!s_vrSystem) return;

	vr::VR_Shutdown();
}
bool ZapMe::VR::VRManager::IsInitialized()
{
	return s_vrSystem != nullptr;
}

std::shared_ptr<ZapMe::VR::Overlay> ZapMe::VR::VRManager::CreateOverlay(const QString& key, const QString& name)
{
	if (!IsInitialized()) return nullptr;

	auto overlay = std::make_shared<ZapMe::VR::Overlay>(key, name);

	if (!overlay->Initialize()) return nullptr;

	s_overlays.push_back(overlay);

	return overlay;
}

void ZapMe::VR::VRManager::RunUpdateLoop()
{
	if (!IsInitialized()) return;

	PollDevicePoses();
}

vr::IVRSystem* ZapMe::VR::VRManager::GetVRSystem()
{
	return s_vrSystem;
}

vr::IVROverlay* ZapMe::VR::VRManager::GetVROverlay()
{
	return s_vrOverlay;
}


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
void PollDevicePoses()
{
	std::bitset<vr::k_unMaxTrackedDeviceCount> validPoses;
	std::bitset<vr::k_unMaxTrackedDeviceCount> connectedDevices;
	for (std::size_t i = 0; i < vr::k_unMaxTrackedDeviceCount;) {
		validPoses[i] = s_devices[i].bPoseIsValid;
		connectedDevices[i] = s_devices[i].bDeviceIsConnected;
	}

	s_vrSystem->GetDeviceToAbsoluteTrackingPose(
		vr::TrackingUniverseStanding,
		0.0f,
		s_devices.data(),
		static_cast<std::uint32_t>(s_devices.size())
	);

	for (std::uint32_t i = 0; i < s_devices.size(); i++)
	{
		const auto& pose = s_devices[i];

		if (connectedDevices[i] != pose.bDeviceIsConnected)
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

		if (validPoses[i] != pose.bPoseIsValid)
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
