#pragma once

#include "transform.h"

#include <QOpenGLWidget>
#include <QOpenGLFramebufferObject>
#include <QOpenGLContext>
#include <QOffscreenSurface>
#include <QGraphicsScene>
#include <QOpenGLPaintDevice>
#include <QPainter>
#include <QString>
#include <QObject>
#include <glm/vec2.hpp>
#include <fmt/core.h>
#include <openvr.h>
#include <memory>
#include <span>

namespace ZapMe::VR {
class Overlay : public QObject {
	Q_OBJECT
	Q_DISABLE_COPY(Overlay)

	friend class VRInputManager;

	Overlay() = delete;
public:
	Overlay(const QString& key, const QString& name, QObject* parent = nullptr);
	~Overlay() override;

	bool IsOk() const {
		return m_handle != vr::k_ulOverlayHandleInvalid;
	}

	QWidget* Widget() const { return m_widget; }

	bool IsVisible() const;
	bool SetIsVisible(bool visible);

	float Width() const { return m_width; }
	bool SetWidth(float width);

	glm::vec2 Size() const { return m_size; }

	Transform Transform() const { return m_transform; }
	bool SetTransform(const ZapMe::VR::Transform& transform);

	enum class TrackedDeviceType {
		HMD,
		AnyController,
		LeftController,
		RightController,
	};
	bool SetTransformRelative(const ZapMe::VR::Transform& transform, Overlay* relativeTo);
	bool SetTransformRelative(const ZapMe::VR::Transform& transform, TrackedDeviceType trackedDeviceType);
	bool SetTransformRelative(const ZapMe::VR::Transform& transform, vr::TrackedDeviceIndex_t trackedDeviceIndex);

	bool FireMouseEvent(Qt::MouseButton button, const glm::vec2& pos);
	void MouseNotPresent();
private:
	void resizeVR(const QRectF& rect);
	void paintVR();

	void destroy();

	QOffscreenSurface* m_surface;
	QGraphicsScene* m_scene;
	QOpenGLContext* m_glctx;
	QWidget* m_widget;
	QGraphicsProxyWidget* m_proxy;

	struct Canvas {
		Canvas(const QSize& size);

		bool Render(QGraphicsScene* scene, vr::IVROverlay* overlay, vr::VROverlayHandle_t overlayHandle);
	private:
		QOpenGLPaintDevice m_paintDevice;
		QOpenGLFramebufferObject m_fbo;
		vr::Texture_t m_tex;
		QPainter m_painter;
	};

	std::unique_ptr<Canvas> m_canvas;

	ZapMe::VR::Transform m_transform;
	glm::vec2 m_size;
	float m_width;

	Qt::MouseButton m_lastMouseButtons;
	QPointF m_lastMousePos;
	bool m_lastMousePresent;

	vr::VROverlayHandle_t m_handle;
	QString m_key;
	QString m_name;
};
}
