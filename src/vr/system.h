#pragma once

#include <openvr.h>
#include <array>

namespace ZapMe::VR {
class Overlay;
struct VRSystem {
	static bool Initialize();
	static void Shutdown();
	static bool IsInitialized();

	static void Update() {
		PollInput();
	}
protected:
	friend class Overlay;
	static void RegisterOverlay(Overlay* overlay);
	static void UnregisterOverlay(Overlay* overlay);
private:
	static void PollInput();
};
}
