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

namespace ZapMe::VR {

class Overlay : public QObject {
	Q_OBJECT
	Q_DISABLE_COPY(Overlay)

	friend class VRManager;
	friend class std::shared_ptr<Overlay>;

	Overlay() = delete;
public:
	Overlay(const QString& key, const QString& name);
	~Overlay() override;

	bool Initialize();
	void Destroy();

	QWidget* GetRootWidget() const { return m_widget; }

	bool SetIsVisible(bool visible);
private:
	void resizeVR(const QRectF& rect);
	void paintVR();

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

	Transform m_transform;
	glm::vec2 m_uvSize;
	glm::vec2 m_uvOriginOffset;

	vr::VROverlayHandle_t m_handle;
	QString m_key;
	QString m_name;
};

}
