#include "tracking_reference.h"

#include <array>

vr::TrackedDeviceClass GetTrackedDeviceClass(ShockLink::VR::TrackingReference::ReferenceType deviceType) {
	switch (deviceType) {
	case ShockLink::VR::TrackingReference::ReferenceType::None:
		return vr::TrackedDeviceClass_Invalid;
	case ShockLink::VR::TrackingReference::ReferenceType::FixedIndex:
		return vr::TrackedDeviceClass_Invalid;
	case ShockLink::VR::TrackingReference::ReferenceType::HMD:
		return vr::TrackedDeviceClass_HMD;
	case ShockLink::VR::TrackingReference::ReferenceType::ControllerAny:
	case ShockLink::VR::TrackingReference::ReferenceType::ControllerLeft:
	case ShockLink::VR::TrackingReference::ReferenceType::ControllerRight:
		return vr::TrackedDeviceClass_Controller;
	case ShockLink::VR::TrackingReference::ReferenceType::Tracker:
		return vr::TrackedDeviceClass_GenericTracker;
	case ShockLink::VR::TrackingReference::ReferenceType::TrackingReference:
		return vr::TrackedDeviceClass_TrackingReference;
	default:
		return vr::TrackedDeviceClass_Invalid;
	}
}

vr::ETrackedControllerRole GetTrackedDeviceControllerRole(ShockLink::VR::TrackingReference::ReferenceType deviceType) {
	switch (deviceType) {
	case ShockLink::VR::TrackingReference::ReferenceType::ControllerAny:
		return vr::TrackedControllerRole_OptOut;
	case ShockLink::VR::TrackingReference::ReferenceType::ControllerLeft:
		return vr::TrackedControllerRole_LeftHand;
	case ShockLink::VR::TrackingReference::ReferenceType::ControllerRight:
		return vr::TrackedControllerRole_RightHand;
	default:
		return vr::TrackedControllerRole_Invalid;
	}
}

vr::TrackedDeviceIndex_t GetTrackedDeviceIndex(ShockLink::VR::TrackingReference::ReferenceType deviceType) {
	switch (deviceType)
	{
	case ShockLink::VR::TrackingReference::ReferenceType::None:
	case ShockLink::VR::TrackingReference::ReferenceType::FixedIndex:
		return vr::k_unTrackedDeviceIndexInvalid;
	case ShockLink::VR::TrackingReference::ReferenceType::HMD:
		return vr::k_unTrackedDeviceIndex_Hmd;
	case ShockLink::VR::TrackingReference::ReferenceType::ControllerAny:
	case ShockLink::VR::TrackingReference::ReferenceType::ControllerLeft:
	case ShockLink::VR::TrackingReference::ReferenceType::ControllerRight:
	case ShockLink::VR::TrackingReference::ReferenceType::Tracker:
	case ShockLink::VR::TrackingReference::ReferenceType::TrackingReference:
		break;
	default:
		return vr::k_unTrackedDeviceIndexInvalid;
	}

	vr::IVRSystem* vrSystem = vr::VRSystem();
	if (vrSystem == nullptr) {
		return vr::k_unTrackedDeviceIndexInvalid;
	}

	vr::TrackedDeviceClass deviceClass = GetTrackedDeviceClass(deviceType);

	static std::array<vr::TrackedDeviceIndex_t, vr::k_unMaxTrackedDeviceCount> devices;
	std::uint32_t nDevices = vrSystem->GetSortedTrackedDeviceIndicesOfClass(deviceClass, devices.data(), devices.size());
	if (nDevices == 0) {
		return vr::k_unTrackedDeviceIndexInvalid;
	}

	vr::ETrackedControllerRole controllerRole = GetTrackedDeviceControllerRole(deviceType);
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

vr::ETrackedControllerRole GetTrackedDeviceControllerRole(vr::TrackedDeviceIndex_t index) {
	vr::IVRSystem* vrSystem = vr::VRSystem();
	if (vrSystem == nullptr) {
		return vr::TrackedControllerRole_Invalid;
	}

	return vrSystem->GetControllerRoleForTrackedDeviceIndex(index);
}

ShockLink::VR::TrackingReference::ReferenceType GetTypeFromIndex(vr::TrackedDeviceIndex_t index) {
	if (index == vr::k_unTrackedDeviceIndex_Hmd) {
		return ShockLink::VR::TrackingReference::ReferenceType::HMD;
	}
	if (index >= vr::k_unTrackedDeviceIndex_Hmd && index < vr::k_unMaxTrackedDeviceCount) {
		return ShockLink::VR::TrackingReference::ReferenceType::FixedIndex;
	}
	return ShockLink::VR::TrackingReference::ReferenceType::None;
}

ShockLink::VR::TrackingReference::TrackingReference(QObject* parent)
	: QObject(parent)
	, m_transform()
	, m_index(vr::k_unTrackedDeviceIndexInvalid)
	, m_class(vr::TrackedDeviceClass_Invalid)
	, m_controllerRole(vr::TrackedControllerRole_Invalid)
	, m_trackingResult(vr::TrackingResult_Uninitialized)
	, m_flags(Flags::None)
{
}

ShockLink::VR::TrackingReference::TrackingReference(vr::TrackedDeviceIndex_t index, QObject* parent)
	: QObject(parent)
	, m_transform()
	, m_index(index)
	, m_class(vr::TrackedDeviceClass_Invalid)
	, m_controllerRole(vr::TrackedControllerRole_Invalid)
	, m_trackingResult(vr::TrackingResult_Uninitialized)
	, m_flags(Flags::None)
{
	if (index == vr::k_unTrackedDeviceIndex_Hmd) {
		m_class = vr::TrackedDeviceClass_HMD;
	}
}

ShockLink::VR::TrackingReference::TrackingReference(ShockLink::VR::TrackingReference::ReferenceType type, QObject* parent)
	: QObject(parent)
	, m_transform()
	, m_index(GetTrackedDeviceIndex(type))
	, m_class(GetTrackedDeviceClass(type))
	, m_controllerRole(GetTrackedDeviceControllerRole(type))
	, m_trackingResult(vr::TrackingResult_Uninitialized)
	, m_flags(Flags::None)
{
}

bool ShockLink::VR::TrackingReference::IsConnected() const
{
	return m_flags & Flags::Connected;
}

bool ShockLink::VR::TrackingReference::IsPoseValid() const
{
	return m_flags & Flags::PoseValid;
}

ShockLink::VR::Transform ShockLink::VR::TrackingReference::Transform() const
{
	return m_transform;
}

vr::TrackedDeviceIndex_t ShockLink::VR::TrackingReference::DeviceIndex() const
{
	return m_index;
}

ShockLink::VR::TrackingReference::ReferenceType ShockLink::VR::TrackingReference::Reference() const
{
	switch (m_class)
	{
	case vr::TrackedDeviceClass_HMD:
		return ReferenceType::HMD;
	case vr::TrackedDeviceClass_Controller:
		switch (m_controllerRole)
		{
		case vr::TrackedControllerRole_LeftHand:
			return ReferenceType::ControllerLeft;
		case vr::TrackedControllerRole_RightHand:
			return ReferenceType::ControllerRight;
		default:
			return ReferenceType::ControllerAny;
		}
	case vr::TrackedDeviceClass_GenericTracker:
		return ReferenceType::Tracker;
	case vr::TrackedDeviceClass_TrackingReference:
		return ReferenceType::TrackingReference;
	default:
		return ReferenceType::None;
	}
}

void ShockLink::VR::TrackingReference::OnDeviceConnected(vr::TrackedDeviceIndex_t deviceIndex, vr::ETrackedDeviceClass deviceClass, const vr::TrackedDevicePose_t& pose)
{
	// If the device is not the specific index we are looking for, or the class is not the one we are looking for, return
	if ((m_class == vr::TrackedDeviceClass_Invalid && m_index != deviceIndex) || m_class != deviceClass) {
		return;
	}

	// If the class is a controller and the role is not opt out, check the role
	if (m_class == vr::TrackedDeviceClass_Controller && m_controllerRole != vr::TrackedControllerRole_OptOut) {
		vr::ETrackedControllerRole controllerRole = GetTrackedDeviceControllerRole(deviceIndex);
		if (m_controllerRole != controllerRole) {
			return;
		}
	}

	SetTransform(pose.mDeviceToAbsoluteTracking);
	SetTrackingResult(pose.eTrackingResult);
	SetConnected(true);
}

void ShockLink::VR::TrackingReference::OnDeviceDisconnected(vr::TrackedDeviceIndex_t deviceIndex)
{
	if (deviceIndex != m_index) return;

	SetPoseValid(false);
	SetConnected(false);
}

void ShockLink::VR::TrackingReference::OnDevicePoseValidated(vr::TrackedDeviceIndex_t deviceIndex, const vr::TrackedDevicePose_t& pose)
{
	if (deviceIndex != m_index) return;

	SetTransform(pose.mDeviceToAbsoluteTracking);
	SetTrackingResult(pose.eTrackingResult);
	SetPoseValid(true);
}

void ShockLink::VR::TrackingReference::OnDevicePoseInvalidated(vr::TrackedDeviceIndex_t deviceIndex)
{
	if (deviceIndex != m_index) return;

	SetPoseValid(false);
}

void ShockLink::VR::TrackingReference::OnDevicePoseUpdated(vr::TrackedDeviceIndex_t deviceIndex, const vr::TrackedDevicePose_t& pose)
{
	if (deviceIndex != m_index) return;

	SetTransform(pose.mDeviceToAbsoluteTracking);
	SetTrackingResult(pose.eTrackingResult);
}

void ShockLink::VR::TrackingReference::SetConnected(bool connected)
{
	std::uint8_t before = m_flags;
	if (connected) {
		m_flags |= Flags::Connected;
	}
	else {
		m_flags &= ~Flags::Connected;
	}

	if (before != m_flags) {
		emit ConnectedChanged(connected);
	}
}

void ShockLink::VR::TrackingReference::SetPoseValid(bool poseValid)
{
	std::uint8_t before = m_flags;
	if (poseValid) {
		m_flags |= Flags::PoseValid;
	}
	else {
		m_flags &= ~Flags::PoseValid;
	}

	if (before != m_flags) {
		emit PoseValidChanged(poseValid);
	}
}

void ShockLink::VR::TrackingReference::SetTransform(const vr::HmdMatrix34_t& transform)
{
	if (m_transform == transform) return;

	m_transform = transform;
	emit TransformChanged(m_transform);
}

void ShockLink::VR::TrackingReference::SetTrackingResult(vr::ETrackingResult trackingResult)
{
	if (m_trackingResult == trackingResult) return;

	m_trackingResult = trackingResult;
	emit TrackingResultChanged(m_trackingResult);
}