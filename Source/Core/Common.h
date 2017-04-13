#pragma once
#include <iostream>
#include <memory>
#ifndef QT_VERSION
    #include <nano-signal-slot/nano_signal_slot.hpp>
    #define PRINT(x) std::cout << x << std::endl;
#endif

constexpr int PAGE_BYTES = 1 << 13; // 8192 (use in OctreeFarm)
const int BLOCK_INFO_END = 1;

struct Size {
    Size(int width, int height) : width(width), height(height) {}
    int width;
    int height;
};
