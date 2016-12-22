#pragma once
#include "Scene2D.h"
#include "../UI/Button.h"
#include "../UI/LinearLayout.h"
#include "../Graphics/Vulkan/Pipeline.h"
#include "../Graphics/Vulkan/PipelineLayout.h"
#include "../Graphics/Vulkan/DescriptorSetLayout.h"
#include "../Graphics/Vulkan/Collection/DescriptorSetCollection.h"
#include "../Graphics/Vulkan/Buffer.h"
#include "../Graphics/Vulkan/DeviceMemory.h"
#include "../Graphics/Vulkan/Collection/CommandBufferCollection.h"

class MenuScene : public Scene2D {

public:
    MenuScene(int width, int height);
    ~MenuScene();
    void draw(float dt) override;
    void update(float dt) override;
    void create() override;

private:
    void onKeyPressed(const SDL_KeyboardEvent& event) override;
    Vulkan::PipelineLayout* pipelineLayout = nullptr;
    Vulkan::DescriptorSetLayout* descriptorSetLayout = nullptr;
    Vulkan::DescriptorSetCollection* descriptorSetCollection = nullptr;
    Vulkan::Pipeline* graphicsPipeline = nullptr;
    Vulkan::Buffer* vertexBuffer = nullptr;
    Vulkan::DeviceMemory* vertexMemory = nullptr;
    Vulkan::CommandBufferCollection* commandBufferCollection = nullptr;
};
