#pragma once

namespace ShockLink::VR {
class Overlay;
class TrackingReference;
struct VRSystem {
	static bool Initialize();
	static void Shutdown();
	static bool IsInitialized();

	static void Update() {
		PollInput();
	}
protected:
	friend class Overlay;
	friend class TrackingReference;
	static void RegisterOverlay(Overlay* overlay);
	static void UnregisterOverlay(Overlay* overlay);
	static void RegisterTrackingReference(TrackingReference* trackingReference);
	static void UnregisterTrackingReference(TrackingReference* trackingReference);
private:
	static void PollInput();
};
}
