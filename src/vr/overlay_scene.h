#pragma once

#include <QOpenGLFramebufferObject>
#include <QOpenGLPaintDevice>
#include <QPainter>
#include <QObject>
#include <QSize>
#include <glm/vec2.hpp>
#include <openvr.h>
#include <memory>

class QWidget;
class QOffscreenSurface;
class QGraphicsScene;
class QOpenGLContext;
class QGraphicsProxyWidget;

namespace ShockLink::VR {
class OverlayScene : public QObject {
	Q_OBJECT
	Q_DISABLE_COPY(OverlayScene)

	OverlayScene() = delete;
public:
	OverlayScene(QObject* parent = nullptr);
	~OverlayScene() override;

	bool Ok() const { return m_scene != nullptr; }
	bool Ready() const { return Ok() && m_handle != vr::k_ulOverlayHandleInvalid && m_canvas != nullptr; }

	vr::VROverlayHandle_t Handle() const;
	QWidget* Widget() const;
	QSize Size() const;

	bool FireMouseEvent(Qt::MouseButton button, const glm::vec2& pos);
	void MouseNotPresent();
public slots:
	void SetHandle(vr::VROverlayHandle_t handle);
	void SetWidget(QWidget* widget);
signals:
	void HandleChanged(vr::VROverlayHandle_t handle);
	void WidgetChanged(QWidget* widget);
	void SizeChanged(const QSize& size);
private:
	void resizeVR(const QRectF& rect);
	void paintVR();

	void removeWidget(QWidget* match);

	vr::VROverlayHandle_t m_handle;
	QOffscreenSurface* m_surface;
	QGraphicsScene* m_scene;
	QOpenGLContext* m_glctx;
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
	QSize m_size;

	Qt::MouseButton m_lastMouseButtons;
	QPointF m_lastMousePos;
	bool m_lastMousePresent;
};
}
