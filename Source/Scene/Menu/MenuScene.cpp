#include "MenuScene.h"
#include "../../Core/App.h"
#include "../../Scene/SceneManager.h"
#include "../../Event/Input.h"
#include "../../Graphics/Vulkan/Instance.h"
#include "../../Graphics/Vulkan/Swapchain.h"
#include "../../Graphics/Vulkan/Framebuffer.h"
#include "../../Resource/ShaderResource.h"
#include "../../Resource/ResourceManager.h"
#include "../../Graphics/Vulkan/Command/CommandBuffer.h"
#include "../../Graphics/Vulkan/Command/CommandBuffers.h"
#include "../../Graphics/Plane.h"
#include "../../Graphics/Vulkan/Queue/SubmitQueue.h"

MenuScene::MenuScene() {
}

MenuScene::~MenuScene() {

}

void MenuScene::init() {
    Scene::init();

    shaderProgram.addShader("Shader/Base.vert.spv");
    shaderProgram.addShader("Shader/Base.frag.spv");

    shaderProgram.linkUniform("uboVert", sizeof(uboVert), &uboVert);
    shaderProgram.linkUniform("uboFrag", sizeof(uboFrag), &uboFrag);

    shaderProgram.linkInput("position", sizeof(glm::vec2));

    vertexBuffer = std::make_shared<Vulkan::Buffer>(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, plane.getVerticesSize(), false);
    vertexBuffer->create();
    vertexBuffer->write(plane.getVertices().data(), plane.getVerticesSize());

    shaderProgram.createIndexBuffer(plane.getIndicesSize());
    shaderProgram.getIndexBuffer()->write(plane.getIndices().data(), plane.getIndicesSize());

    shaderProgram.createPipeline();

    buildCommandBuffers();

    shaderProgram.writeUniform("uboVert");
    shaderProgram.writeUniform("uboFrag");
}

void MenuScene::draw(float dt) {
    queue->submit();
}

void MenuScene::update(float dt) {

}

void MenuScene::create() {

}

void MenuScene::writeCommands(Vulkan::CommandBuffer* commandBuffer) {
    VkRenderPassBeginInfo* renderPassBeginInfo = &Vulkan::Instance::get()->getSurface()->getSwapchain()->getRenderPass()->beginInfo;
    VkExtent2D extent = Vulkan::Instance::get()->getSurface()->getCurrentExtent();
    renderPassBeginInfo->renderArea.extent = extent;

    commandBuffer->beginRenderPass(renderPassBeginInfo);

    commandBuffer->bindPipeline(shaderProgram.getGraphicsPipeline());
    commandBuffer->addVertexBuffer(vertexBuffer->getHandle());
    commandBuffer->bindVertexBuffers();
    commandBuffer->bindIndexBuffer(shaderProgram.getIndexBuffer()->getHandle());

    for (int i = 0; i < shaderProgram.getDescriptorSets()->getCount(); i++) {
        commandBuffer->addDescriptorSet(shaderProgram.getDescriptorSets()->at(i));
    }

    commandBuffer->bindDescriptorSets(shaderProgram.getGraphicsPipeline()->getBindPoint(), shaderProgram.getPipelineLayout()->getHandle());
    commandBuffer->drawIndexed(plane.getIndices().size(), 1, 0, 0, 0);

    commandBuffer->endRenderPass();
}

void MenuScene::onKeyPressed(const SDL_KeyboardEvent& event) {
    if (event.keysym.sym == SDLK_ESCAPE) {
        SceneManager::get()->popScene();
        Input::get()->isKeyAccepted = true;
    }
}
