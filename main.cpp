#include "vr/vr_manager.h"

#include <fmt/core.h>

int main() {
    ZapMe::VRManager vr_manager;

    vr_manager.CreateOverlay("test", "test");

    return 0;
}