#pragma once
#include "Queue.h"
#include <vector>

namespace Vulkan {

class PresentQueue : public Queue {

public:
    PresentQueue(const Device* device, uint32_t queueFamilyIndex, uint32_t queueIndex = 0);
    VkResult present();
    void addSwapchain(VkSwapchainKHR swapchain);

    VkPresentInfoKHR presentInfo = {};

private:
    std::vector<VkSwapchainKHR> swapchains;
};

} // Vulkan
