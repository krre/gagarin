#pragma once
#include "Graphics/Render/RenderWindow.h"
#include "Graphics/Vulkan/Wrapper/Surface/Surface.h"
#include "Graphics/Vulkan/Wrapper/Surface/Swapchain.h"

class VulkanRenderWindow : public RenderWindow {

public:
    VulkanRenderWindow();

    void clear() override;
    void swapBuffers() override;
    void saveImage(const std::string& filePath) override;

    Vulkan::Surface* getSurface() const { return surface.get(); }
    Vulkan::Swapchain* getSwapchain() const { return swapchain.get(); }

private:
    void setColorBackend(const Color& color) override;

    std::unique_ptr<Vulkan::Surface> surface;
    std::unique_ptr<Vulkan::Swapchain> swapchain;
};
