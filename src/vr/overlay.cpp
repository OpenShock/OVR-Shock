#include "vr/overlay.h"

#include "system.h"

#include <QApplication>
#include <QGraphicsProxyWidget>
#include <QGraphicsSceneEvent>

ZapMe::VR::Overlay::Overlay(const QString& key, const QString& name, QObject* parent)
	: QObject(parent)
	, m_surface(new QOffscreenSurface(nullptr, this))
	, m_scene(new QGraphicsScene(this))
	, m_glctx(new QOpenGLContext(this))
	, m_proxy(nullptr)
	, m_canvas(nullptr)
	, m_transform()
	, m_size(1.0f, 1.0f)
	, m_width(1.0f)
	, m_lastMouseButtons(Qt::NoButton)
	, m_lastMousePos(0.0f, 0.0f)
	, m_lastMousePresent(false)
	, m_handle(vr::k_ulOverlayHandleInvalid)
	, m_key(key)
	, m_name(name)
{
	// OpenGL context
	QSurfaceFormat format;
	format.setMajorVersion(4); // TODO: Fiddle with this
	format.setMinorVersion(1); // TODO: Fiddle with this
	format.setProfile(QSurfaceFormat::CompatibilityProfile); // TODO: Fiddle with this
	m_glctx->setFormat(format);
	if (!m_glctx->create()) {
		fmt::print("Failed to create OpenGL context\n");
		destroy();
		return;
	}

	// Surface and widget
	m_surface->create();

	// VR Overlay
	auto overlay = vr::VROverlay();
	if (overlay == nullptr) {
		fmt::print("Failed to get IVROverlay interface\n");
		destroy();
		return;
	}

	vr::EVROverlayError error;
	error = overlay->CreateOverlay(m_key.toLatin1().data(), m_name.toLatin1().data(), &m_handle);
	if (error != vr::VROverlayError_None) {
		fmt::print("Failed to create overlay: {}\n", overlay->GetOverlayErrorNameFromEnum(error));
		destroy();
		return;
	}

	error = overlay->SetOverlayWidthInMeters(m_handle, 1.0f);
	if (error != vr::VROverlayError_None) {
		fmt::print("Failed to set overlay width: {}\n", overlay->GetOverlayErrorNameFromEnum(error));
		destroy();
		return;
	}

	// Connect signals
	connect(m_scene, &QGraphicsScene::sceneRectChanged, this, &Overlay::resizeVR);
	connect(m_scene, &QGraphicsScene::changed, this, &Overlay::paintVR);

	// Register overlay
	VRSystem::RegisterOverlay(this);
}

ZapMe::VR::Overlay::~Overlay() {
	destroy();
}

QWidget* ZapMe::VR::Overlay::Widget() const {
	if (m_proxy == nullptr) {
		return nullptr;
	}

	return m_proxy->widget();
}

bool ZapMe::VR::Overlay::SetWidget(QWidget* widget) {
	removeWidget(nullptr);
	if (widget == nullptr) {
		return true;
	}

	widget->move(0, 0);
	widget->setMouseTracking(true);
	widget->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);

	m_proxy = m_scene->addWidget(widget);

	connect(widget, &QObject::destroyed, [this, widget]() { removeWidget(widget); });

	return true;
}

bool ZapMe::VR::Overlay::IsVisible() const {
	auto overlay = vr::VROverlay();
	if (overlay == nullptr) {
		fmt::print("Failed to get IVROverlay interface\n");
		return false;
	}

	return overlay->IsOverlayVisible(m_handle);
}

bool ZapMe::VR::Overlay::SetIsVisible(bool visible) {
	auto overlay = vr::VROverlay();
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

bool ZapMe::VR::Overlay::SetWidth(float width) {
	auto overlay = vr::VROverlay();
	if (overlay == nullptr) {
		fmt::print("Failed to get IVROverlay interface\n");
		return false;
	}

	vr::EVROverlayError error;
	error = overlay->SetOverlayWidthInMeters(m_handle, width);
	if (error != vr::VROverlayError_None) {
		fmt::print("Failed to set overlay width: {}\n", overlay->GetOverlayErrorNameFromEnum(error));
		return false;
	}

	m_width = width;

	return true;
}

bool ZapMe::VR::Overlay::SetTransform(const ZapMe::VR::Transform& transform) {
	auto overlay = vr::VROverlay();
	if (overlay == nullptr) {
		fmt::print("Failed to get IVROverlay interface\n");
		return false;
	}

	vr::HmdMatrix34_t matrix = transform.ToHmdMatrix();

	vr::EVROverlayError error;
	error = overlay->SetOverlayTransformAbsolute(m_handle, vr::TrackingUniverseStanding, &matrix);
	if (error != vr::VROverlayError_None) {
		fmt::print("Failed to set overlay transform: {}\n", overlay->GetOverlayErrorNameFromEnum(error));
		return false;
	}

	return true;
}

bool ZapMe::VR::Overlay::SetTransformRelative(const ZapMe::VR::Transform& transform, Overlay* relativeTo) {
	if (relativeTo == nullptr || relativeTo == this) {
		return SetTransform(transform);
	}

	auto overlay = vr::VROverlay();
	if (overlay == nullptr) {
		fmt::print("Failed to get IVROverlay interface\n");
		return false;
	}

	vr::HmdMatrix34_t matrix = transform.ToHmdMatrix();

	// TODO: This feature seems to be broken in OpenVR (And they don't seem to care about providing support for individual developers at all, so I won't bother trying to get it fixed)
	vr::EVROverlayError error;
	error = overlay->SetOverlayTransformOverlayRelative(m_handle, relativeTo->m_handle, &matrix);
	if (error != vr::VROverlayError_None) {
		fmt::print("Failed to set overlay transform: {}\n", overlay->GetOverlayErrorNameFromEnum(error));
		return false;
	}

	vr::VROverlayTransformType transformType;
	error = overlay->GetOverlayTransformType(m_handle, &transformType);
	if (error != vr::VROverlayError_None) {
		fmt::print("Failed to get overlay transform type: {}\n", overlay->GetOverlayErrorNameFromEnum(error));
		return false;
	}

	return true;
}

bool ZapMe::VR::Overlay::SetTransformRelative(const ZapMe::VR::Transform& transform, TrackedDeviceType trackedDeviceType) {
	if (trackedDeviceType == TrackedDeviceType::HMD) {
		return SetTransformRelative(transform, vr::k_unTrackedDeviceIndex_Hmd);
	}
	
	vr::ETrackedControllerRole controllerRole;
	switch (trackedDeviceType) {
		case TrackedDeviceType::AnyController:
			controllerRole = vr::TrackedControllerRole_OptOut;
			break;
		case TrackedDeviceType::LeftController:
			controllerRole = vr::TrackedControllerRole_LeftHand;
			break;
		case TrackedDeviceType::RightController:
			controllerRole = vr::TrackedControllerRole_RightHand;
			break;
		default:
			fmt::print("Invalid tracked device type\n");
			return false;
	}

	auto system = vr::VRSystem();
	auto overlay = vr::VROverlay();
	if (system == nullptr || overlay == nullptr) {
		fmt::print("Failed to get IVRSystem or IVROverlay interface\n");
		return false;
	}

	vr::HmdMatrix34_t matrix = transform.ToHmdMatrix();

	vr::TrackedDeviceIndex_t trackedDeviceIndex = system->GetTrackedDeviceIndexForControllerRole(controllerRole);

	vr::EVROverlayError error;
	error = overlay->SetOverlayTransformTrackedDeviceRelative(m_handle, trackedDeviceIndex, &matrix);
	if (error != vr::VROverlayError_None) {
		fmt::print("Failed to set overlay transform: {}\n", overlay->GetOverlayErrorNameFromEnum(error));
		return false;
	}

	return true;
}

bool ZapMe::VR::Overlay::SetTransformRelative(const ZapMe::VR::Transform& transform, vr::TrackedDeviceIndex_t trackedDeviceIndex) {
	auto overlay = vr::VROverlay();
	if (overlay == nullptr) {
		fmt::print("Failed to get IVROverlay interface\n");
		return false;
	}

	vr::HmdMatrix34_t matrix = transform.ToHmdMatrix();

	vr::EVROverlayError error;
	error = overlay->SetOverlayTransformTrackedDeviceRelative(m_handle, trackedDeviceIndex, &matrix);
	if (error != vr::VROverlayError_None) {
		fmt::print("Failed to set overlay transform: {}\n", overlay->GetOverlayErrorNameFromEnum(error));
		return false;
	}

	return true;
}

bool ZapMe::VR::Overlay::FireMouseEvent(Qt::MouseButton button, const glm::vec2& pos) {
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
	QApplication::sendEvent(widget, &mouseEvent);

	m_lastMouseButtons = button;
	m_lastMousePos = pt;

	return true;
}

void ZapMe::VR::Overlay::resizeVR(const QRectF& rect) {
	m_glctx->makeCurrent(m_surface);
	m_canvas = std::make_unique<Canvas>(rect.size().toSize());
	m_glctx->doneCurrent();

	m_size = glm::vec2(m_width, m_width * (rect.height() / rect.width()));
}

void ZapMe::VR::Overlay::paintVR() {
	auto overlay = vr::VROverlay();
	if (overlay == nullptr) {
		return;
	}

	m_glctx->makeCurrent(m_surface);
	m_canvas->Render(m_scene, overlay, m_handle);
	m_glctx->doneCurrent();
}

void ZapMe::VR::Overlay::removeWidget(QWidget* match) {
	if (m_proxy == nullptr) return;

	QWidget* widget = m_proxy->widget();
	if (widget == nullptr || (match != nullptr && widget != match)) return;

	disconnect(widget, nullptr, this, nullptr);
	widget->setMouseTracking(false);
	widget->setWindowFlags(Qt::Widget);

	m_scene->removeItem(m_proxy);
	m_proxy->deleteLater();
	m_proxy = nullptr;
}

void ZapMe::VR::Overlay::destroy() {
	VRSystem::UnregisterOverlay(this);

	if (m_handle != vr::k_ulOverlayHandleInvalid) {
		auto overlay = vr::VROverlay();
		if (overlay == nullptr) {
			fmt::print("Failed to get IVROverlay interface\n");
		} else {
			overlay->DestroyOverlay(m_handle);
		}

		m_handle = vr::k_ulOverlayHandleInvalid;
	}

	removeWidget(nullptr);
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
		fmt::print("Failed to set overlay texture: {}\n", vr::VROverlay()->GetOverlayErrorNameFromEnum(error));
		return false;
	}

	return true;
}