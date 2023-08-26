#include "vr/overlay.h"

#include "vr_manager.h"

ZapMe::VR::Overlay::Overlay(const QString& key, const QString& name)
	: QObject()
	, m_surface(new QOffscreenSurface(nullptr, this))
	, m_scene(new QGraphicsScene(this))
	, m_glctx(new QOpenGLContext(this))
	, m_widget(new QWidget(nullptr))
	, m_proxy(nullptr)
	, m_canvas(nullptr)
	, m_transform()
	, m_uvSize(1.0f)
	, m_uvOriginOffset(0.0f)
	, m_handle(0)
	, m_key(key)
	, m_name(name)
{
	QSurfaceFormat format;
	format.setMajorVersion(4); // TODO: Fiddle with this
	format.setMinorVersion(1); // TODO: Fiddle with this
	format.setProfile(QSurfaceFormat::CompatibilityProfile); // TODO: Fiddle with this

	m_glctx->setFormat(format);

	if (!m_glctx->create()) {
		fmt::print("Failed to create OpenGL context\n");
		exit(1);
		return;
	}

	m_surface->create();

	m_widget->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
	m_widget->move(0, 0);
	m_proxy = m_scene->addWidget(m_widget);

	connect(m_scene, &QGraphicsScene::sceneRectChanged, this, &Overlay::resizeVR);
	connect(m_scene, &QGraphicsScene::changed, this, &Overlay::paintVR);

	// Destruction
	connect(m_glctx, &QOpenGLContext::aboutToBeDestroyed, this, &Overlay::Destroy);
}

ZapMe::VR::Overlay::~Overlay() {
	m_widget->deleteLater();
}

bool ZapMe::VR::Overlay::Initialize() {
	auto overlay = VRManager::GetVROverlay();
	if (overlay == nullptr) {
		fmt::print("Failed to get IVROverlay interface\n");
		return false;
	}

	vr::EVROverlayError error;
	error = overlay->CreateOverlay(m_key.toLatin1().data(), m_name.toLatin1().data(), &m_handle);
	if (error != vr::VROverlayError_None) {
		fmt::print("Failed to create overlay: {}\n", overlay->GetOverlayErrorNameFromEnum(error));
		return false;
	}

	error = overlay->SetOverlayWidthInMeters(m_handle, 1.0f);
	if (error != vr::VROverlayError_None) {
		fmt::print("Failed to set overlay width: {}\n", overlay->GetOverlayErrorNameFromEnum(error));
		return false;
	}

	return true;
}

void ZapMe::VR::Overlay::Destroy() {
	auto overlay = VRManager::GetVROverlay();
	if (overlay == nullptr) {
		fmt::print("Failed to get IVROverlay interface\n");
		return;
	}

	overlay->DestroyOverlay(m_handle);
}

bool ZapMe::VR::Overlay::SetIsVisible(bool visible) {
	auto overlay = VRManager::GetVROverlay();
	if (overlay == nullptr) {
		fmt::print("Failed to get IVROverlay interface\n");
		return false;
	}

	vr::EVROverlayError error;
	if (visible) {
		error = overlay->ShowOverlay(m_handle);
	} else {
		error = overlay->HideOverlay(m_handle);
	}
	if (error != vr::VROverlayError_None) {
		fmt::print("Failed to set overlay visibility: {}\n", overlay->GetOverlayErrorNameFromEnum(error));
		return false;
	}

	return true;
}

void ZapMe::VR::Overlay::resizeVR(const QRectF& rect) {
	m_glctx->makeCurrent(m_surface);
	m_canvas = std::make_unique<Canvas>(rect.size().toSize());
}

void ZapMe::VR::Overlay::paintVR() {
	auto overlay = VRManager::GetVROverlay();
	if (overlay == nullptr) {
		return;
	}

	m_glctx->makeCurrent(m_surface);
	m_canvas->Render(m_scene, overlay, m_handle);
	m_glctx->doneCurrent();
}

ZapMe::VR::Overlay::Canvas::Canvas(const QSize& size)
	: m_paintDevice(size)
	, m_fbo(size, GL_TEXTURE_2D)
	, m_tex()
	, m_painter(&m_paintDevice)
{
	m_tex.eType = vr::TextureType_OpenGL;
	m_tex.eColorSpace = vr::ColorSpace_Auto;
	m_painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);
}

bool ZapMe::VR::Overlay::Canvas::Render(QGraphicsScene* scene, vr::IVROverlay* overlay, vr::VROverlayHandle_t overlayHandle) {
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
		fmt::print("Failed to set overlay texture: {}\n", VRManager::GetVROverlay()->GetOverlayErrorNameFromEnum(error));
		return false;
	}

	return true;
}