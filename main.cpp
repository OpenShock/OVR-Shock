#include "vr/system.h"
#include "vr/overlay.h"

#include <QApplication>
#include <QLabel>

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

    auto overlay = new ZapMe::VR::Overlay("test", "test", &app);
    if (!overlay->IsOk()) {
        fmt::print("Failed to create overlay\n");
        return 1;
    }

    auto overlay2 = new ZapMe::VR::Overlay("test2", "test2", &app);
    if (!overlay->IsOk()) {
        fmt::print("Failed to create overlay 2\n");
        return 1;
    }

    overlay->Widget()->setStyleSheet("QWidget { background: black; }");
    overlay->SetWidth(0.2f);
    overlay->SetIsVisible(true);

    overlay2->Widget()->setStyleSheet("QWidget { background: red; }");
    overlay2->SetWidth(0.2f);
    overlay2->SetIsVisible(true);

    overlay->SetTransformRelative(ZapMe::VR::Transform(glm::vec3(0.0f, 0.0f, 0.1f), glm::radians(glm::vec3(0.0f, 0.0f, 45.0f))), ZapMe::VR::Overlay::TrackedDeviceType::LeftController);
    overlay->SetTransformRelative(ZapMe::VR::Transform(glm::vec3(0.0f, 0.0f, 0.1f), glm::radians(glm::vec3(0.0f, 0.0f, 45.0f))), ZapMe::VR::Overlay::TrackedDeviceType::RightController);

    return app.exec();
}