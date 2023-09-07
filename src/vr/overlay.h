#pragma once

#include "tracking_reference.h"
#include "transform.h"

#include <QObject>
#include <QString>
#include <QPointF>
#include <glm/vec2.hpp>
#include <openvr.h>

class QWidget;

namespace ShockLink::VR {

class OverlayScene;

class Overlay : public QObject {
	Q_OBJECT
	Q_DISABLE_COPY(Overlay)

	enum CriticalError : std::uint16_t {
		None = 0,
		TransformFailed = 1 << 0,
		DeviceNotFound = 1 << 1,
		InvalidDeviceIndex = 1 << 2,

		TranformIssue = TransformFailed | DeviceNotFound | InvalidDeviceIndex
	};

	Overlay() = delete;
public:
	Overlay(const QString& key, const QString& name, QObject* parent = nullptr);
	~Overlay() override;

	bool Ok() const;
	bool Visible() const;
	float Width() const { return m_width; }
	glm::vec2 Size() const { return m_size; }
	bool TransformOk() const { return m_criticalErrors != CriticalError::None; }
	ShockLink::VR::Transform Transform() const { return m_transform; }
	ShockLink::VR::TrackingReference* TransformReference() const { return m_transformReference; }
	ShockLink::VR::OverlayScene* Scene() const { return m_scene; }
signals:
	void TransformOkChanged(bool hasError);
	void VisibleChanged(bool visible);
	void WidthChanged(float width);
	void TransformChanged(const ShockLink::VR::Transform& transform);
	void TransformReferenceChanged(ShockLink::VR::TrackingReference* reference);
public slots:
	void SetVisible(bool visible);
	void SetWidth(float width);
	void SetTransform(const ShockLink::VR::Transform& transform);
	void SetTransformReference(ShockLink::VR::TrackingReference* reference);
private slots:
	void OnTransformReferenceMoved(const ShockLink::VR::Transform& transform);
private:
	void SetCriticalError(CriticalError error);
	void ClearCriticalError(CriticalError error);
	void ClearCriticalErrors();
	bool HasCriticalError(CriticalError error) const { return (m_criticalErrors & error) != 0; }
	void InternalSetVisible(bool visible);
	bool InternalTransformVR();

	bool ResolveCriticalErrors();
	bool ResolveInvalidTransformReference();

	vr::VROverlayHandle_t m_handle;
	ShockLink::VR::OverlayScene* m_scene;
	ShockLink::VR::Transform m_transform;
	ShockLink::VR::TrackingReference* m_transformReference;
	std::uint16_t m_criticalErrors;
	QString m_key;
	QString m_name;
	glm::vec2 m_size;
	float m_width;
};
}
