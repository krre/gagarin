#pragma once
#include "Graphics/Render/RenderManager.h"

namespace Vulkan {
    class Instance;
}

class VulkanRenderManager : public RenderManager {

public:
    VulkanRenderManager();
    ~VulkanRenderManager();
    std::shared_ptr<RenderWindow> createRenderWindow() override;

private:
    std::unique_ptr<Vulkan::Instance> vulkan;
};