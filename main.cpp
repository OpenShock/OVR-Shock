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
    ShockLink::VR::VRSystem::Shutdown();
}

int main(int argc, char** argv) {
    std::atexit(at_exit);
    QCoreApplication::setAttribute(Qt::AA_UseDesktopOpenGL);
    QApplication app(argc, argv);

    if (!ShockLink::VR::VRSystem::Initialize()) {
        fmt::print("Failed to initialize OpenVR\n");
        return 1;
    }

    auto overlayA = new ShockLink::VR::Overlay("testA", "testA", &app);
    if (!overlayA->Ok()) {
        fmt::print("Failed to create overlay\n");
        return 1;
    }

    auto overlayB = new ShockLink::VR::Overlay("testB", "testB", &app);
    if (!overlayB->Ok()) {
        fmt::print("Failed to create overlay 2\n");
        return 1;
    }

    ShockLink::VRWidget* widgetA = new ShockLink::VRWidget();
    widgetA->setStyleSheet("QWidget { background-color: gray; }");

    ShockLink::VRWidget* widgetB = new ShockLink::VRWidget();
    widgetB->setStyleSheet("QWidget { background-color: gray; }");

    overlayA->Scene()->SetWidget(widgetA);
    overlayA->SetWidth(0.2f);
    overlayA->SetVisible(true);

    overlayB->Scene()->SetWidget(widgetB);
    overlayB->SetWidth(0.2f);
    overlayB->SetVisible(true);

    ShockLink::VR::Transform controllerOffset;
    controllerOffset.SetPosition(glm::vec3(0.0f, 0.05f, 0.1f));
    controllerOffset.SetRotation(glm::radians(glm::vec3(-90.0f, 0.0f, 0.0f)));

    //overlayA->SetTransformRelative(controllerOffset, ShockLink::VR::Overlay::TrackedDeviceType::LeftController);
    //overlayB->SetTransformRelative(controllerOffset, ShockLink::VR::Overlay::TrackedDeviceType::RightController);

    QTimer* vrLoopTimer = new QTimer(&app);
    QObject::connect(vrLoopTimer, &QTimer::timeout, [&]() {
		ShockLink::VR::VRSystem::Update();
	});
    vrLoopTimer->start(10);

    return app.exec();
}