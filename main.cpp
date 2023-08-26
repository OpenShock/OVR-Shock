#include "vr/vr_manager.h"

#include <QApplication>
#include <QLabel>

#include <fmt/core.h>

#include <chrono>
#include <thread>

void at_exit() {
}

int main(int argc, char** argv) {
    QCoreApplication::setAttribute(Qt::AA_UseDesktopOpenGL);
    QApplication app(argc, argv);

    if (!ZapMe::VR::VRManager::Initialize()) {
		fmt::print("Failed to initialize OpenVR\n");
		return 1;
	}

    auto overlay = ZapMe::VR::VRManager::CreateOverlay("test", "test");
    if (!overlay) {
        fmt::print("Failed to create overlay\n");
        ZapMe::VR::VRManager::Shutdown();
        return 1;
    }

    // Create example text with Qt and render it to the overlay
    auto parent = new QWidget();
    parent->setStyleSheet("QWidget { background: red; }");
    parent->show();

    overlay->GetRootWidget()->setStyleSheet("QWidget { background: red; }");
    overlay->SetIsVisible(true);

    int rv = app.exec();

    // Sleep for 2 minutes
    std::this_thread::sleep_for(std::chrono::minutes(2));

    ZapMe::VR::VRManager::Shutdown();

    return rv;
}