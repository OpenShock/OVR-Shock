#pragma once

#include <memory>

#include "transform.h"

#include <QString>

#include <fmt/core.h>

#include <openvr.h>

#include <glm/vec2.hpp>

namespace ZapMe {

struct Overlay
{
	static std::shared_ptr<Overlay> Create(vr::IVROverlay* ptr, const QString& key, const QString& name, vr::EVROverlayError& error)
	{
		vr::VROverlayHandle_t handle;
		error = ptr->CreateOverlay(key.toLatin1().data(), name.toLatin1().data(), &handle);
		if (error != vr::VROverlayError_None) {
			return nullptr;
		}

		return std::make_shared<Overlay>(ptr, handle, key, name);
	}

	void Destroy()
	{
		ptr->DestroyOverlay(handle);
	}

	Transform transform;
	glm::vec2 uvSize;
	glm::vec2 uvOriginOffset;

	vr::IVROverlay* ptr;
	vr::VROverlayHandle_t handle;
	QString key;
	QString name;
};

}
