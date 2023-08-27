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

    // Create example text with Qt and render it to the overlay
    auto parent = new QWidget();
    parent->setStyleSheet("QWidget { background: red; }");
    parent->show();

    overlay->Widget()->setStyleSheet("QWidget { background: black; }");
    overlay->SetWidth(0.2f);
    overlay->SetIsVisible(true);




    return app.exec();
}