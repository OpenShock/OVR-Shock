#pragma once

#include "vr/transform.h"

#include <QObject>

#include <openvr.h>
#include <cstdint>

namespace ShockLink::VR {
	class TrackingReference : public QObject {
		Q_OBJECT
		Q_DISABLE_COPY(TrackingReference)

		TrackingReference() = delete;
	public:
		enum class ReferenceType : std::uint8_t
		{
			None,
			FixedIndex,
			HMD,
			ControllerAny,
			ControllerLeft,
			ControllerRight,
			Tracker,
			TrackingReference,
		};

		TrackingReference(QObject* parent = nullptr);
		TrackingReference(ReferenceType type, QObject* parent = nullptr);
		TrackingReference(vr::TrackedDeviceIndex_t index, QObject* parent = nullptr);

		bool IsConnected() const;
		bool IsPoseValid() const;
		ShockLink::VR::Transform Transform() const;
		vr::TrackedDeviceIndex_t DeviceIndex() const;
		ReferenceType Reference() const;
	signals:
		void ConnectedChanged(bool connected);
		void PoseValidChanged(bool poseValid);
		void TransformChanged(const ShockLink::VR::Transform& transform);
		void TrackingResultChanged(vr::ETrackingResult trackingResult);
	public slots:
		void OnDeviceConnected(vr::TrackedDeviceIndex_t deviceIndex, vr::ETrackedDeviceClass deviceClass, const vr::TrackedDevicePose_t& pose);
		void OnDeviceDisconnected(vr::TrackedDeviceIndex_t deviceIndex);
		void OnDevicePoseValidated(vr::TrackedDeviceIndex_t deviceIndex, const vr::TrackedDevicePose_t& pose);
		void OnDevicePoseInvalidated(vr::TrackedDeviceIndex_t deviceIndex);
		void OnDevicePoseUpdated(vr::TrackedDeviceIndex_t deviceIndex, const vr::TrackedDevicePose_t& pose);
	private:
		void SetConnected(bool connected);
		void SetPoseValid(bool poseValid);
		void SetTransform(const vr::HmdMatrix34_t& pose);
		void SetTrackingResult(vr::ETrackingResult trackingResult);

		ShockLink::VR::Transform m_transform;
		vr::TrackedDeviceIndex_t m_index;
		vr::TrackedDeviceClass m_class;
		vr::ETrackedControllerRole m_controllerRole;
		vr::ETrackingResult m_trackingResult;
		std::uint8_t m_flags;

		enum Flags : std::uint8_t {
			None = 0,
			Connected  = 1 << 0,
			PoseValid  = 1 << 1,
		};
	};
}