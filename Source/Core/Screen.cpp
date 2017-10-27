#include "Screen.h"
#include "UI/View.h"
#include "Graphics/Render/RenderManager.h"
#include "Core/Application.h"
#include "Graphics/Render/RenderWindow.h"
#include "Graphics/Vulkan/Context.h"
#include "Graphics/Vulkan/Semaphore.h"
#include "Graphics/Vulkan/Queue/SubmitQueue.h"
#include "Graphics/Vulkan/Surface/Swapchain.h"
#include "Graphics/Vulkan/Command/CommandBuffers.h"
#include "Graphics/Vulkan/Command/CommandBuffer.h"

Screen::Screen() {
    device = Vulkan::Context::get()->getGraphicsDevice();

    commandBufferHandlers = std::make_unique<Vulkan::CommandBuffers>(device, Vulkan::Context::get()->getGraphicsCommandPool());
    commandBufferHandlers->allocate(Application::get()->getWindow()->getSwapchain()->getCount());

    for (int i = 0; i < commandBufferHandlers->getCount(); i++) {
        auto commandBuffer = std::make_unique<Vulkan::CommandBuffer>(commandBufferHandlers->at(i));
        commandBuffers.push_back(std::move(commandBuffer));
    }

    renderFinishedSemaphore = std::make_unique<Vulkan::Semaphore>(device);
    renderFinishedSemaphore->create();

    submitQueue = std::make_unique<Vulkan::SubmitQueue>(device, Vulkan::Context::get()->getGraphicsFamily());
    submitQueue->create();
}

Screen::~Screen() {

}

void Screen::hide() {

}

void Screen::pause() {
    isPaused = true;
}

void Screen::resume() {
    isPaused = false;
}

void Screen::show() {

}

void Screen::update(float dt) {
    for (auto* renderView : renderViews) {
        renderView->update(dt);
    }
}

void Screen::render() {
    Application::get()->getWindow()->acquireNextImage();

    submitQueue->clearCommandBuffers();
    uint32_t imageIndex = Application::get()->getWindow()->getSwapchain()->getImageIndex();
    submitQueue->addCommandBuffer(commandBuffers.at(imageIndex).get());
//    submitQueue->submit();

    Application::get()->getWindow()->present();
}

void Screen::resize(uint32_t width, uint32_t height) {
    for (const auto& view : views) {
        view->resize(width, height);
    }
}

void Screen::pushView(const std::shared_ptr<View>& view) {
    views.push_back(view);
    currentView = view.get();
    updateRenderViews();
}

void Screen::popView() {
    views.pop_back();
    currentView = views.size() ? views.back().get() : nullptr;
    updateRenderViews();
}

void Screen::updateRenderViews() {
    for (const auto view : views) {
        for (const auto& innerView : view->getInnerViews()) {
            renderViews.push_back(innerView);
        }
    }

    for (auto* renderView : renderViews) {
        PRINT(renderView)
    }
}
