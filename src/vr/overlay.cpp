#include "vr/overlay.h"

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
	auto overlay = vr::VROverlay();
	if (overlay == nullptr) {
		fmt::print("Failed to get IVROverlay interface\n");
		return;
	}

	vr::EVROverlayError error;
	if (visible) {
		error = overlay->ShowOverlay(m_handle);
	} else {
		error = overlay->HideOverlay(m_handle);
	}

	if (error != vr::VROverlayError_None) {
		fmt::print("Failed to set overlay visibility: {}\n", overlay->GetOverlayErrorNameFromEnum(error));
		return;
	}

	emit VisibleChanged(visible);
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