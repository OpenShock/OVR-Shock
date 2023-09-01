#pragma once

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

	Overlay() = delete;
public:
	Overlay(const QString& key, const QString& name, QObject* parent = nullptr);
	~Overlay() override;

	bool Ok() const;
	bool Visible() const;
	float Width() const { return m_width; }
	glm::vec2 Size() const { return m_size; }
	OverlayScene* Scene() const { return m_scene; }
public slots:
	void SetVisible(bool visible);
	void SetWidth(float width);
signals:
	void VisibleChanged(bool visible);
	void WidthChanged(float width);
private:
	vr::VROverlayHandle_t m_handle;
	OverlayScene* m_scene;
	QString m_key;
	QString m_name;
	glm::vec2 m_size;
	float m_width;
};
}
