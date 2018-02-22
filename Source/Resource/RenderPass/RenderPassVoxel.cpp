#include "RenderPassVoxel.h"
#include "Graphics/Color.h"
#include "Base/Game.h"
#include "Base/Window.h"
#include "Vulkan/API/RenderPass.h"
#include "Vulkan/API/Framebuffer.h"
#include "Vulkan/API/Image/ImageView.h"
#include "Vulkan/API/Command/CommandBuffer.h"
#include "Vulkan/API/Surface/Surface.h"
#include "Vulkan/API/RenderPass.h"
#include "Vulkan/API/Descriptor/DescriptorSets.h"
#include "Vulkan/API/Pipeline/PipelineLayout.h"
#include "Graphics/Render/RenderManager.h"
#include "Vulkan/ShaderProgram.h"
#include "Vulkan/GpuBuffer.h"
#include "Vulkan/API/Pipeline/GraphicsPipeline.h"
#include "Resource/ResourceManager.h"
#include "ECS/Scenes/Scene.h"
#include "UI/Font.h"
#include <glm/gtc/matrix_transform.hpp>

namespace Origin {

RenderPassVoxel::RenderPassVoxel(Vulkan::Device* device, Object* parent) :
        RenderPassResource(device, parent) {
    renderPass = std::make_unique<Vulkan::RenderPass>(device);
    renderPass->setColorFormat(Game::getRenderManager()->getSurface()->getFormats().at(0).format);
    renderPass->setDepthEnable(true);
    renderPass->setDepthFormat(VK_FORMAT_D16_UNORM); // TODO: Take from render pass used for framebuffer
    renderPass->create();

    uint32_t startSize = 1000000; // TODO: Set optimal value or take from constant
    vertexBuffer = std::make_unique<Vulkan::GpuBuffer>(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, startSize);

    uboBuffer = std::make_unique<Vulkan::GpuBuffer>(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, sizeof(glm::mat4));

    shaderProgram = std::make_unique<Vulkan::ShaderProgram>();
    shaderProgram->loadShader(Game::getResourceManager()->getDataPath() + "/Shader/Voxel.vert.spv");
    shaderProgram->loadShader(Game::getResourceManager()->getDataPath() + "/Shader/Voxel.frag.spv");

    VkDescriptorBufferInfo bufferInfo = {};
    bufferInfo.buffer = uboBuffer->getHandle();
    bufferInfo.range = VK_WHOLE_SIZE;
    shaderProgram->bindBuffer("ubo", bufferInfo);

    shaderProgram->create();

    graphicsPipeline = std::make_unique<Vulkan::GraphicsPipeline>(device);
    graphicsPipeline->setRenderPass(renderPass->getHandle());
    graphicsPipeline->setPipelineLayout(shaderProgram->getPipelineLayout()->getHandle());

    graphicsPipeline->addDynamicState(VK_DYNAMIC_STATE_VIEWPORT);
    graphicsPipeline->addDynamicState(VK_DYNAMIC_STATE_SCISSOR);

    for (auto& shader : shaderProgram->getShaders()) {
        graphicsPipeline->addShaderCode(shader->getStage(), shader->getCode().size() * sizeof(uint32_t), shader->getCode().data(), "main");
    }

    VkVertexInputBindingDescription bindingDescription;
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(Scene::Vertex);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    graphicsPipeline->addVertexBindingDescription(bindingDescription);

    {
        const Vulkan::Shader::LocationInfo locationInfo = shaderProgram->getLocationInfo("position");
        VkVertexInputAttributeDescription attributeDescription = {};
        attributeDescription.binding = bindingDescription.binding;
        attributeDescription.location = locationInfo.location;
        attributeDescription.format = locationInfo.format;
        graphicsPipeline->addVertexAttributeDescription(attributeDescription);
    }

    {
        const Vulkan::Shader::LocationInfo locationInfo = shaderProgram->getLocationInfo("color");
        VkVertexInputAttributeDescription attributeDescription = {};
        attributeDescription.binding = bindingDescription.binding;
        attributeDescription.location = locationInfo.location;
        attributeDescription.format = locationInfo.format;
        attributeDescription.offset = sizeof(Scene::Vertex::pos);
        graphicsPipeline->addVertexAttributeDescription(attributeDescription);
    }

    graphicsPipeline->create();
}

RenderPassVoxel::~RenderPassVoxel() {

}

void RenderPassVoxel::write(Vulkan::CommandBuffer* commandBuffer, Vulkan::Framebuffer* framebuffer) {
    const Color& color = Game::getWindow()->getColor();

    // TODO: Only need update on resize framebuffer
    glm::mat4 mvp = glm::ortho(0.0f, (float)framebuffer->getWidth(), (float)framebuffer->getHeight(), 0.0f);
    uboBuffer->write(&mvp, sizeof(mvp));

    Vulkan::RenderPassBegin renderPassBegin(renderPass->getHandle());
    renderPassBegin.setFrameBuffer(framebuffer->getHandle());
    renderPassBegin.setRenderArea({ 0, 0, framebuffer->getWidth(), framebuffer->getHeight() });
    renderPassBegin.addClearValue({ color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha() });
    VkClearValue depthColor = {};
    depthColor.depthStencil.depth = 1.0f;
    depthColor.depthStencil.stencil = 0.0f;
    renderPassBegin.addClearValue(depthColor);

    commandBuffer->beginRenderPass(renderPassBegin.getInfo());

    if (vertextCount) {
        commandBuffer->bindPipeline(graphicsPipeline.get());

        commandBuffer->addVertexBuffer(vertexBuffer->getHandle());
        commandBuffer->bindVertexBuffers();

        for (int i = 0; i < shaderProgram->getDescriptorSets()->getCount(); i++) {
            commandBuffer->addDescriptorSet(shaderProgram->getDescriptorSets()->at(i));
        }
        commandBuffer->bindDescriptorSets(graphicsPipeline->getBindPoint(), shaderProgram->getPipelineLayout()->getHandle());

        commandBuffer->draw(vertextCount, 1, 0, 0);
    }

    commandBuffer->endRenderPass();
}

void RenderPassVoxel::resizeVertexBuffer(uint32_t size) {
    vertexBuffer = std::make_unique<Vulkan::GpuBuffer>(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, size);
}

} // Origin