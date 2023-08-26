#pragma once

#include "vr/overlay.h"

#include <QString>
#include <openvr.h>

namespace ZapMe::VR {
struct VRManager {
	static bool Initialize();
	static void Shutdown();
	static bool IsInitialized();

	static std::shared_ptr<Overlay> CreateOverlay(const QString& key, const QString& name);
		
	static void RunUpdateLoop();
protected:
	friend class Overlay;

	static vr::IVRSystem* GetVRSystem();
	static vr::IVROverlay* GetVROverlay();
};
}
