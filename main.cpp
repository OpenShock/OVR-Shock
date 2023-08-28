#include "vr/system.h"
#include "vr/overlay.h"

#include <QApplication>
#include <QLabel>
#include <QMovie>

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
    if (!overlayA->IsOk()) {
        fmt::print("Failed to create overlay\n");
        return 1;
    }

    auto overlayB = new ZapMe::VR::Overlay("testB", "testB", &app);
    if (!overlayB->IsOk()) {
        fmt::print("Failed to create overlay 2\n");
        return 1;
    }

    QMovie* movie = new QMovie("F:/Hydrus/fc5/c57270d1e60f9bd716e047a59f49836cf28d862bfd7ede9fdab5a8eb19daf534.gif");
    if (!movie->isValid()) {
		fmt::print("Failed to load movie\n");
		return 1;
	}
    movie->setCacheMode(QMovie::CacheAll);

    QLabel* labelA = new QLabel();
    labelA->setMovie(movie);

    QLabel* labelB = new QLabel();
    labelB->setMovie(movie);

    overlayA->SetWidget(labelA);
    overlayA->SetWidth(0.2f);
    overlayA->SetIsVisible(true);

    overlayB->SetWidget(labelB);
    overlayB->SetWidth(0.2f);
    overlayB->SetIsVisible(true);

    movie->start();

    overlayA->SetTransformRelative(ZapMe::VR::Transform(glm::vec3(0.0f, 0.0f, 0.1f), glm::radians(glm::vec3(0.0f, 0.0f, 45.0f))), ZapMe::VR::Overlay::TrackedDeviceType::LeftController);
    overlayB->SetTransformRelative(ZapMe::VR::Transform(glm::vec3(0.0f, 0.0f, 0.1f), glm::radians(glm::vec3(0.0f, 0.0f, 45.0f))), ZapMe::VR::Overlay::TrackedDeviceType::RightController);

    return app.exec();
}