#pragma once

#include "vr/overlay.h"

#include <fmt/core.h>

#include <openvr.h>

#include <array>
#include <bitset>
#include <vector>

namespace ZapMe
{
	class VRManager
	{
	public:
		VRManager()
		{
			vr::EVRInitError error;
			m_vrSystem = vr::VR_Init(&error, vr::VRApplication_Overlay);
			if (error != vr::VRInitError_None)
			{
				m_vrSystem = nullptr;
				return;
			}

			m_vrOverlay = vr::VROverlay();
		}

		bool CreateOverlay(const QString& key, const QString& name)
		{
			if (!m_vrSystem)
			{
				return false;
			}

			vr::EVROverlayError error;
			auto overlay = Overlay::Create(m_vrOverlay, key, name, error);
			if (error != vr::VROverlayError_None) {
				fmt::print("Failed to create overlay: {}\n", m_vrOverlay->GetOverlayErrorNameFromEnum(error));
				return false;
			}

			m_overlays.push_back(std::move(overlay));
			return true;
		}

		void pollDevicePoses()
		{
			std::bitset<vr::k_unMaxTrackedDeviceCount> validPoses;
			std::bitset<vr::k_unMaxTrackedDeviceCount> connectedDevices;
			for (std::size_t i = 0; i < vr::k_unMaxTrackedDeviceCount;) {
				validPoses[i] = m_devicePoses[i].bPoseIsValid;
				connectedDevices[i] = m_devicePoses[i].bDeviceIsConnected;
			}

			m_vrSystem->GetDeviceToAbsoluteTrackingPose(
				vr::TrackingUniverseStanding,
				0.0f,
				m_devicePoses.data(),
				static_cast<std::uint32_t>(m_devicePoses.size())
			);

			for (std::uint32_t i = 0; i < m_devicePoses.size(); i++)
			{
				const auto& pose = m_devicePoses[i];

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
		
		void update()
		{
			if (!m_vrSystem)
			{
				return;
			}

			pollDevicePoses();
		}

		void OnDeviceConnected(std::uint32_t index, const vr::TrackedDevicePose_t& pose) const
		{
			fmt::print("Device {} connected\n", index);
		}
		void OnDeviceDisconnected(std::uint32_t index) const
		{
			fmt::print("Device {} disconnected\n", index);
		}
		void OnDevicePoseValidated(std::uint32_t index, const vr::TrackedDevicePose_t& pose) const
		{
			fmt::print("Device {} pose validated\n", index);
		}
		void OnDevicePoseInvalidated(std::uint32_t index) const
		{
			fmt::print("Device {} pose invalidated\n", index);
		}
		void OnDevicePoseUpdated(std::uint32_t index, const vr::TrackedDevicePose_t& pose) const
		{
			fmt::print("Device {} pose updated\n", index);
		}
	private:
		vr::IVRSystem* m_vrSystem;
		vr::IVROverlay* m_vrOverlay;
		std::array<vr::TrackedDevicePose_t, vr::k_unMaxTrackedDeviceCount> m_devicePoses;
		std::vector<std::shared_ptr<Overlay>> m_overlays;
	};
}
