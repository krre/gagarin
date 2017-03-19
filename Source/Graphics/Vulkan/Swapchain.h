#pragma once
#include "Base/Handle.h"
#include "Device/Devicer.h"
#include "Surface.h"
#include <vector>

namespace Vulkan {

class Swapchain : public Handle<VkSwapchainKHR>, public Devicer {

public:
    Swapchain(const Device* device, const Surface* surface);
    ~Swapchain();
    VkResult create() override;
    void destroy() override;
    VkImage getImage(int i) const { return images.at(i); }
    int getImageCount() const { return images.size(); }
    int getIndex() const { return index; }

    VkSwapchainCreateInfoKHR createInfo = {};

private:
    const Surface* surface;
    std::vector<VkImage> images;
    int index = -1;
    static int indexCounter;
};

} // Vulkan
