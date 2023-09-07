#include "vr/overlay.h"

#include "vr/tracked_device.h"
#include "vr/overlay_scene.h"
#include "vr/system.h"

#include <QApplication>
#include <QGraphicsProxyWidget>
#include <QGraphicsSceneEvent>
#include <fmt/core.h>

ShockLink::VR::Overlay::Overlay(const QString& key, const QString& name, QObject* parent)
	: QObject(parent)
	, m_handle(vr::k_ulOverlayHandleInvalid)
	, m_scene(new OverlayScene(this))
	, m_key(key)
	, m_name(name)
	, m_size(1.0f, 1.0f)
	, m_width(1.0f)
{
	// VR Overlay
	auto overlay = vr::VROverlay();
	if (overlay == nullptr) {
		fmt::print("Failed to get IVROverlay interface\n");
		return;
	}

	vr::EVROverlayError error;
	error = overlay->CreateOverlay(m_key.toLatin1().data(), m_name.toLatin1().data(), &m_handle);
	if (error != vr::VROverlayError_None) {
		fmt::print("Failed to create overlay: {}\n", overlay->GetOverlayErrorNameFromEnum(error));
		m_handle = vr::k_ulOverlayHandleInvalid;
		return;
	}

	m_scene->SetHandle(m_handle);

	// Register overlay
	VRSystem::RegisterOverlay(this);
}

ShockLink::VR::Overlay::~Overlay() {
	// Unregister overlay
	VRSystem::UnregisterOverlay(this);

	if (m_handle != vr::k_ulOverlayHandleInvalid) {
		auto overlay = vr::VROverlay();
		if (overlay == nullptr) {
			fmt::print("Failed to get IVROverlay interface\n");
		} else {
			overlay->DestroyOverlay(m_handle);
		}
	}
}

bool ShockLink::VR::Overlay::Ok() const {
	return m_handle != vr::k_ulOverlayHandleInvalid && m_scene != nullptr && m_scene->Ok();
}

bool ShockLink::VR::Overlay::Visible() const {
	auto overlay = vr::VROverlay();
	if (overlay == nullptr) {
		fmt::print("Failed to get IVROverlay interface\n");
		return false;
	}

	return overlay->IsOverlayVisible(m_handle);
}

void ShockLink::VR::Overlay::SetVisible(bool visible) {
	if (m_criticalErrors != CriticalError::None) {
		return;
	}
	InternalSetVisible(visible);
}

void ShockLink::VR::Overlay::SetWidth(float width) {
	auto overlay = vr::VROverlay();
	if (overlay == nullptr) {
		fmt::print("Failed to get IVROverlay interface\n");
		return;
	}

	vr::EVROverlayError error;
	error = overlay->SetOverlayWidthInMeters(m_handle, width);
	if (error != vr::VROverlayError_None) {
		fmt::print("Failed to set overlay width: {}\n", overlay->GetOverlayErrorNameFromEnum(error));
		return;
	}

	m_width = width;

	emit WidthChanged(width);
}

// Checked OK
void ShockLink::VR::Overlay::SetTransform(const ShockLink::VR::Transform& transform) {
	if (m_transform != transform) {
		m_transform = transform;
		emit TransformChanged(transform);
	}

	InternalTransformVR();
}

// Checked OK
void ShockLink::VR::Overlay::SetTransformReference(ShockLink::VR::TrackingReference* reference) {
	if (m_transformReference != reference)
	{
		QObject::disconnect(m_transformReference, nullptr, this, nullptr);
		m_transformReference->deleteLater();
		m_transformReference = reference;

		if (reference != nullptr)
		{
			QObject::connect(reference, &TrackingReference::TransformChanged, this, &Overlay::OnTransformReferenceMoved);
		}

		emit TransformReferenceChanged(reference);
	}

	InternalTransformVR();
}

void ShockLink::VR::Overlay::InternalSetVisible(bool visible) {
	auto overlay = vr::VROverlay();
	if (overlay == nullptr) {
		fmt::print("Failed to get IVROverlay interface\n");
		return;
	}

	vr::EVROverlayError error;
	if (visible) {
		error = overlay->ShowOverlay(m_handle);
	}
	else {
		error = overlay->HideOverlay(m_handle);
	}

	if (error != vr::VROverlayError_None) {
		fmt::print("Failed to set overlay visibility: {}\n", overlay->GetOverlayErrorNameFromEnum(error));
		return;
	}

	emit VisibleChanged(visible);
}

void ShockLink::VR::Overlay::SetCriticalError(CriticalError error) {
	bool ok = TransformOk();

	m_criticalErrors |= error;

	bool after = TransformOk();
	if (ok != after) {
		emit TransformOkChanged(after);
	}
}

void ShockLink::VR::Overlay::ClearCriticalError(CriticalError error) {
	bool ok = TransformOk();

	m_criticalErrors &= ~error;

	bool after = TransformOk();

	if (ok != after) {
		emit TransformOkChanged(after);
	}
}

void ShockLink::VR::Overlay::ClearCriticalErrors() {
	bool ok = TransformOk();

	m_criticalErrors = CriticalError::None;

	bool after = TransformOk();

	if (ok != after) {
		emit TransformOkChanged(after);
	}
}

bool ShockLink::VR::Overlay::InternalTransformVR() {
	auto overlay = vr::VROverlay();
	if (overlay == nullptr) {
		fmt::print("Failed to get IVROverlay interface\n");
		return false;
	}

	if (deviceIndex == vr::k_unTrackedDeviceIndexInvalid) {
		fmt::print("Invalid device index\n");
		m_criticalErrors |= CriticalError::InvalidDeviceIndex;
		InternalSetVisible(false);
		return false;
	}

	auto hmdMat = transform.ToHmdMatrix();

	vr::EVROverlayError error = overlay->SetOverlayTransformTrackedDeviceRelative(m_handle, deviceIndex, &hmdMat);
	if (error != vr::VROverlayError_None) {
		InternalSetVisible(false);
		if (error == vr::VROverlayError_InvalidTrackedDevice) {
			fmt::print("Invalid device index\n");
			InternalUpdateTrackedDeviceIndex(vr::k_unTrackedDeviceIndexInvalid);
			m_criticalErrors |= CriticalError::InvalidDeviceIndex;
		} else {
			fmt::print("Failed to set overlay transform: {}\n", overlay->GetOverlayErrorNameFromEnum(error));
			m_criticalErrors |= CriticalError::TransformFailed;
		}
		return false;
	}
	
	return true;
}

bool ShockLink::VR::Overlay::ResolveCriticalErrors() {
	if (m_criticalErrors & CriticalError::InvalidDeviceIndex) {
		if (m_trackedDeviceIndex != vr::k_unTrackedDeviceIndexInvalid) {
			InternalUpdateTrackedDeviceIndex(m_trackedDeviceIndex);
			ClearCriticalError(CriticalError::InvalidDeviceIndex);
		}

		m_criticalErrors &= ~CriticalError::InvalidDeviceIndex;
	}

	if (m_criticalErrors & CriticalError::TransformFailed) {
		// Try to set the transform again
		if (!InternalSetTransform(m_transform, m_trackedDeviceIndex)) {
			return false;
		}
	}

	if (m_criticalErrors & CriticalError::DeviceNotFound) {
		// Try to find the device type again
		if (m_transformReference == ShockLink::VR::ReferenceType::SpecificIndex) {
			vr::TrackedDeviceIndex_t deviceIndex = ShockLink::VR::TrackedDevice::GetIndex(m_transformReference);
			if (deviceIndex != vr::k_unTrackedDeviceIndexInvalid) {
				InternalUpdateTransformReference(ShockLink::VR::ReferenceType::None);
				InternalSetVisible(false);
				return false;
			}
		}
		else {
			InternalUpdateTransformReference(ShockLink::VR::ReferenceType::None);
			InternalSetVisible(false);
			return false;
		}
	}

	return true;
}

bool ShockLink::VR::Overlay::ResolveInvalidTransformReference() {
	// If the transform reference is None, then we don't need to do anything
	// Just ensure that the transform reference type is set to None
	if (m_transformReference == ShockLink::VR::ReferenceType::None) {
		InternalUpdateTrackedDeviceIndex(vr::k_unTrackedDeviceIndexInvalid);
		return true;
	}

	if (m_trackedDeviceIndex != vr::k_unTrackedDeviceIndexInvalid) {
		return true;
	}

	// If the transform reference is a specific index, but that index is invalid, then hide the overlay
	// There really isn't anything else we can do, since we don't know what device to use
	if (m_transformReference == ShockLink::VR::ReferenceType::SpecificIndex) {
		InternalUpdateTransformReference(ShockLink::VR::ReferenceType::None);
		m_criticalErrors |= CriticalError::InvalidDeviceIndex;
		InternalSetVisible(false);
		return false;
	}

	// If the transform reference is a specific device type, but that device type is invalid, try to find the device type again
	// If we can't find the device type, then hide the overlay with the reason DeviceTypeNotFound
	vr::TrackedDeviceIndex_t deviceIndex = ShockLink::VR::TrackedDevice::GetIndex(m_transformReference);
	if (deviceIndex == vr::k_unTrackedDeviceIndexInvalid) {
		m_criticalErrors |= CriticalError::DeviceNotFound;
		InternalSetVisible(false);
		return false;
	}

	InternalUpdateTrackedDeviceIndex(deviceIndex);
	if (!InternalSetTransform(m_transform, deviceIndex)) {
		return false;
	}

	m_criticalErrors &= ~CriticalError::TranformIssue;

	return true;
}