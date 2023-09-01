#include "vr/overlay_scene.h"

#include <QApplication>
#include <QWidget>
#include <QOffscreenSurface>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsProxyWidget>
#include <QOpenGLContext>

#include <fmt/core.h>

ShockLink::VR::OverlayScene::OverlayScene(QObject* parent)
	: QObject(parent)
	, m_proxy(), m_canvas(), m_size()
	, m_handle(vr::k_ulOverlayHandleInvalid)
	, m_lastMouseButtons(Qt::NoButton)
	, m_lastMousePos(0.0f, 0.0f)
	, m_lastMousePresent(false)
{
	// OpenGL context
	QSurfaceFormat format;
	format.setMajorVersion(4); // TODO: Fiddle with this
	format.setMinorVersion(1); // TODO: Fiddle with this
	format.setProfile(QSurfaceFormat::CompatibilityProfile); // TODO: Fiddle with this

	m_glctx = new QOpenGLContext(this);
	m_glctx->setFormat(format);
	if (!m_glctx->create()) {
		fmt::print("Failed to create OpenGL context\n");
		return;
	}

	// Surface
	m_surface = new QOffscreenSurface(nullptr, this);
	m_surface->create();

	// Connect signals
	m_scene = new QGraphicsScene(this);
	connect(m_scene, &QGraphicsScene::sceneRectChanged, this, &OverlayScene::resizeVR);
	connect(m_scene, &QGraphicsScene::changed, this, &OverlayScene::paintVR);
}

ShockLink::VR::OverlayScene::~OverlayScene() {
	removeWidget(nullptr);
}

vr::VROverlayHandle_t ShockLink::VR::OverlayScene::Handle() const {
	return m_handle;
}

QWidget* ShockLink::VR::OverlayScene::Widget() const {
	if (m_proxy == nullptr) {
		return nullptr;
	}

	return m_proxy->widget();
}

QSize ShockLink::VR::OverlayScene::Size() const {
	return m_size;
}

void ShockLink::VR::OverlayScene::SetHandle(vr::VROverlayHandle_t handle) {
	if (!Ok() || m_handle == handle) return;

	m_handle = handle;

	emit HandleChanged(m_handle);
}

void ShockLink::VR::OverlayScene::SetWidget(QWidget* widget) {
	if (!Ok() || (m_proxy != nullptr && m_proxy->widget() == widget)) {
		return;
	}

	removeWidget(nullptr);
	if (widget == nullptr) {
		emit WidgetChanged(nullptr);
		return;
	}

	widget->move(0, 0);
	widget->setMouseTracking(true);
	widget->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);

	m_proxy = m_scene->addWidget(widget);

	connect(widget, &QObject::destroyed, [this, widget]() { removeWidget(widget); });

	emit WidgetChanged(widget);
}

void ShockLink::VR::OverlayScene::resizeVR(const QRectF& rect) {
	if (!Ready()) return;

	m_size = rect.size().toSize();

	m_glctx->makeCurrent(m_surface);
	m_canvas = std::make_unique<Canvas>(m_size);
	m_glctx->doneCurrent();

	emit SizeChanged(m_size);
}

void ShockLink::VR::OverlayScene::paintVR() {
	if (!Ready()) return;

	auto overlay = vr::VROverlay();
	if (overlay == nullptr) {
		return;
	}

	m_glctx->makeCurrent(m_surface);
	m_canvas->Render(m_scene, overlay, m_handle);
	m_glctx->doneCurrent();
}

void ShockLink::VR::OverlayScene::removeWidget(QWidget* match) {
	if (m_proxy == nullptr) return;

	QWidget* widget = m_proxy->widget();
	if (widget == nullptr || (match != nullptr && widget != match)) return;

	disconnect(widget, nullptr, this, nullptr);
	widget->setMouseTracking(false);
	widget->setWindowFlags(Qt::Widget);

	if (m_scene == nullptr) return;

	m_scene->removeItem(m_proxy);
	m_proxy->deleteLater();
	m_proxy = nullptr;
}

ShockLink::VR::OverlayScene::Canvas::Canvas(const QSize& size)
	: m_paintDevice(size)
	, m_fbo(size, GL_TEXTURE_2D)
	, m_tex()
	, m_painter(&m_paintDevice)
{
	m_tex.eType = vr::TextureType_OpenGL;
	m_tex.eColorSpace = vr::ColorSpace_Auto;
	m_painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);
}

bool ShockLink::VR::OverlayScene::Canvas::Render(QGraphicsScene* scene, vr::IVROverlay* overlay, vr::VROverlayHandle_t overlayHandle) {
	if (!m_fbo.bind()) {
		fmt::print("Failed to bind framebuffer\n");
		return false;
	}

	scene->render(&m_painter);

	m_tex.handle = (void*)m_fbo.texture();
	auto error = overlay->SetOverlayTexture(overlayHandle, &m_tex);

	if (!m_fbo.release()) {
		fmt::print("Failed to release framebuffer\n");
		return false;
	}

	if (error != vr::VROverlayError_None) {
		fmt::print("Failed to set overlay texture: {}\n", vr::VROverlay()->GetOverlayErrorNameFromEnum(error));
		return false;
	}

	return true;
}

bool ShockLink::VR::OverlayScene::FireMouseEvent(Qt::MouseButton button, const glm::vec2& pos) {
	QWidget* widget = Widget();
	if (widget == nullptr) {
		return false;
	}

	QPointF pt(pos.x, pos.y);

	QGraphicsSceneMouseEvent mouseEvent(QEvent::GraphicsSceneMouseMove);
	mouseEvent.setModifiers(Qt::NoModifier);
	mouseEvent.setButtons(m_lastMouseButtons);
	mouseEvent.setButtonDownPos(button, pt);
	mouseEvent.setLastPos(m_lastMousePos);
	mouseEvent.setPos(pt);
	QApplication::sendEvent((QObject*)widget, &mouseEvent);

	m_lastMouseButtons = button;
	m_lastMousePos = pt;

	return true;
}