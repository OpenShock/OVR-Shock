#include "vr/system.h"

#include "vr/overlay.h"
#include "vr/overlay_scene.h"

#include "vrwidget.h"

#include <QApplication>
#include <QTimer>

#include <fmt/core.h>

#include <chrono>
#include <thread>

void at_exit() {
    ZapMe::VR::VRSystem::Shutdown();
}

int main(int argc, char** argv) {
    std::atexit(at_exit);
    QCoreApplication::setAttribute(Qt::AA_UseDesktopOpenGL);
    QApplication app(argc, argv);

    if (!ZapMe::VR::VRSystem::Initialize()) {
        fmt::print("Failed to initialize OpenVR\n");
        return 1;
    }

    auto overlayA = new ZapMe::VR::Overlay("testA", "testA", &app);
    if (!overlayA->Ok()) {
        fmt::print("Failed to create overlay\n");
        return 1;
    }

    auto overlayB = new ZapMe::VR::Overlay("testB", "testB", &app);
    if (!overlayB->Ok()) {
        fmt::print("Failed to create overlay 2\n");
        return 1;
    }

    ZapMe::VRWidget* widgetA = new ZapMe::VRWidget();
    widgetA->setStyleSheet("QWidget { background-color: gray; }");

    ZapMe::VRWidget* widgetB = new ZapMe::VRWidget();
    widgetB->setStyleSheet("QWidget { background-color: gray; }");

    overlayA->Scene()->SetWidget(widgetA);
    overlayA->SetWidth(0.2f);
    overlayA->SetVisible(true);

    overlayB->Scene()->SetWidget(widgetB);
    overlayB->SetWidth(0.2f);
    overlayB->SetVisible(true);

    ZapMe::VR::Transform controllerOffset;
    controllerOffset.SetPosition(glm::vec3(0.0f, 0.05f, 0.1f));
    controllerOffset.SetRotation(glm::radians(glm::vec3(-90.0f, 0.0f, 0.0f)));

    //overlayA->SetTransformRelative(controllerOffset, ZapMe::VR::Overlay::TrackedDeviceType::LeftController);
    //overlayB->SetTransformRelative(controllerOffset, ZapMe::VR::Overlay::TrackedDeviceType::RightController);

    QTimer* vrLoopTimer = new QTimer(&app);
    QObject::connect(vrLoopTimer, &QTimer::timeout, [&]() {
		ZapMe::VR::VRSystem::Update();
	});
    vrLoopTimer->start(10);

    return app.exec();
}