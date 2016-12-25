#include "MenuScene.h"
#include "../Core/App.h"
#include "../GameState/GameStateManager.h"
#include "../Event/Input.h"
#include "../Graphics/Vulkan/Manager.h"
#include "../Resource/ShaderResource.h"
#include "../Resource/ResourceManager.h"
#include "../Graphics/Vulkan/CommandBuffer.h"

MenuScene::MenuScene(int width, int height) :
    Scene2D(width, height) {
}

MenuScene::~MenuScene() {
    delete commandBufferCollection;
    delete graphicsPipeline;
    delete pipelineLayout;
    delete descriptorSetCollection;
    delete descriptorPool;
    delete descriptorSetLayout;
    delete uniformVert;
    delete uniformFrag;
    delete indexMemoryBuffer;
    delete vertexMemoryBuffer;
}

void MenuScene::draw(float dt) {

}

void MenuScene::update(float dt) {

}

void MenuScene::create() {
    const std::vector<glm::vec2> vertices = {
        { -1.0f,  -1.0f },
        {  1.0f, -1.0f },
        {  1.0f,  1.0f },
        {  -1.0f, 1.0f },
    };

    const std::vector<uint16_t> indices = {
        0, 1, 2, 2, 3, 0
    };

    vertexMemoryBuffer = new Vulkan::MemoryBuffer(device, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
    vertexMemoryBuffer->create(sizeof(vertices[0]) * vertices.size());
    vertexMemoryBuffer->update(vertices.data());

    indexMemoryBuffer = new Vulkan::MemoryBuffer(device, VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
    indexMemoryBuffer->create(sizeof(indices[0]) * indices.size());
    indexMemoryBuffer->update(indices.data());

    uniformVert = new Vulkan::MemoryBuffer(device, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
    uniformVert->create(sizeof(uboVert));
    uniformVert->update(&uboVert);

    uniformFrag = new Vulkan::MemoryBuffer(device, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
    uniformFrag->create(sizeof(uboFrag));
    uniformFrag->update(&uboFrag);

    descriptorPool = new Vulkan::DescriptorPool(device);
    VkDescriptorPoolSize poolSizeUniform = {};
    poolSizeUniform.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizeUniform.descriptorCount = 2;
    descriptorPool->setPoolSizes({ poolSizeUniform });
    descriptorPool->setMaxSets(2);
    descriptorPool->create();

    descriptorSetLayout = new Vulkan::DescriptorSetLayout(device);

    VkDescriptorSetLayoutBinding layoutBingingVert = {};
    layoutBingingVert.binding = 0;
    layoutBingingVert.descriptorCount = 1;
    layoutBingingVert.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    layoutBingingVert.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    VkDescriptorSetLayoutBinding layoutBingingFrag = {};
    layoutBingingFrag.binding = 1;
    layoutBingingFrag.descriptorCount = 1;
    layoutBingingFrag.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    layoutBingingFrag.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    descriptorSetLayout->setBindings({ layoutBingingVert, layoutBingingFrag });
    descriptorSetLayout->create();

    descriptorSetCollection = new Vulkan::DescriptorSetCollection(device, descriptorPool);
    descriptorSetCollection->setDescriptorSetLayouts({ descriptorSetLayout->getHandle() });
    descriptorSetCollection->allocate();
    descriptorSetCollection->update({ uniformVert->getBuffer(), uniformFrag->getBuffer() });

    pipelineLayout = new Vulkan::PipelineLayout(device);
    pipelineLayout->setDescriptorSetLayouts({ descriptorSetLayout->getHandle() });
    pipelineLayout->create();

    graphicsPipeline = new Vulkan::GraphicsPipeline(device);

    ShaderResource* shaderResource = ResourceManager::get()->getResource<ShaderResource>("BaseVertShader");
    graphicsPipeline->addShaderCode(VK_SHADER_STAGE_VERTEX_BIT, "main", (size_t)shaderResource->getSize(), (uint32_t*)shaderResource->getData());

    shaderResource = ResourceManager::get()->getResource<ShaderResource>("BaseFragShader");
    graphicsPipeline->addShaderCode(VK_SHADER_STAGE_FRAGMENT_BIT, "main", (size_t)shaderResource->getSize(), (uint32_t*)shaderResource->getData());

    VkVertexInputBindingDescription bindingDescription = {};
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(glm::vec2);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    graphicsPipeline->setVertexBindingDescriptions({ bindingDescription });

    VkVertexInputAttributeDescription attributeDescriptions = {};
    attributeDescriptions.binding = 0;
    attributeDescriptions.location = 0;
    attributeDescriptions.format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions.offset = 0;
    graphicsPipeline->setVertexAttributeDescriptions({ attributeDescriptions });

    graphicsPipeline->setExtent(Vulkan::Manager::get()->getSwapchain()->getExtent());
    graphicsPipeline->setPipelineLayout(pipelineLayout);
    graphicsPipeline->setRenderPass(Vulkan::Manager::get()->getRenderPass());
    graphicsPipeline->create();

    commandBufferCollection = new Vulkan::CommandBufferCollection(device, Vulkan::Manager::get()->getCommandPool());
    commandBufferCollection->allocate(Vulkan::Manager::get()->getSwapchain()->getImageCount());

    for (size_t i = 0; i < commandBufferCollection->getCount(); i++) {
        Vulkan::CommandBuffer commandBuffer(commandBufferCollection->at(i));
        commandBuffer.begin();

        VkClearValue clearColor = { 0.77, 0.83, 0.83, 1.0 };

        VkRenderPassBeginInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = Vulkan::Manager::get()->getRenderPass()->getHandle();
        renderPassInfo.framebuffer = Vulkan::Manager::get()->getFramebuffer(i)->getHandle();
        renderPassInfo.renderArea.offset = { 0, 0 };
        renderPassInfo.renderArea.extent = Vulkan::Manager::get()->getSwapchain()->getExtent();
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;

        vkCmdBeginRenderPass(commandBuffer.getHandle(), &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(commandBuffer.getHandle(), VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline->getHandle());

        VkBuffer vertexBuffers[] = { vertexMemoryBuffer->getBuffer()->getHandle() };
        VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(commandBuffer.getHandle(), 0, 1, vertexBuffers, offsets);

        vkCmdBindIndexBuffer(commandBuffer.getHandle(), indexMemoryBuffer->getBuffer()->getHandle(), 0, VK_INDEX_TYPE_UINT16);

        vkCmdBindDescriptorSets(commandBuffer.getHandle(), VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout->getHandle(), 0, descriptorSetCollection->getCount(), descriptorSetCollection->getData(), 0, nullptr);
        vkCmdDrawIndexed(commandBuffer.getHandle(), indices.size(), 1, 0, 0, 0);
        vkCmdEndRenderPass(commandBuffer.getHandle());

        commandBuffer.end();
    }

    Vulkan::Manager::get()->setCommandBuffers(commandBufferCollection->getCount(), commandBufferCollection->getData());
}

void MenuScene::onKeyPressed(const SDL_KeyboardEvent& event) {
    if (event.keysym.sym == SDLK_ESCAPE) {
        GameStateManager::get()->popState();
        Input::get()->isKeyAccepted = true;
    }
}
