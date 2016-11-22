#pragma once
#include "VkObject.h"
#include "Device.h"
#include "Surface.h"
#include <vulkan/vulkan.h>
#include <vector>

namespace Vulkan {

class Swapchain : public VkObject {

public:
    Swapchain(const Device* device, const Surface* surface);
    ~Swapchain();
    bool isValid() const { return handle != VK_NULL_HANDLE; }
    VkSwapchainKHR getHandle() const { return handle; }

private:
    VkSwapchainKHR handle = VK_NULL_HANDLE;
    const Device* device;
    const Surface* surface;
    std::vector<VkImage> images;
};

} // Vulkan
