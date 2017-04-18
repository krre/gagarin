#include "CommandBufferOneTime.h"
#include "Graphics/Vulkan/Instance.h"
#include "Graphics/Vulkan/Fence.h"
#include "Graphics/Vulkan/Queue/SubmitQueue.h"

using namespace Vulkan;

CommandBufferOneTime::CommandBufferOneTime(Device* device) :
        device(device) {
    commandBuffers = std::make_shared<CommandBuffers>(Instance::get()->getCommandPool());
    commandBuffers->allocate(1);
    commandBuffer = std::make_shared<CommandBuffer>(commandBuffers->at(0));
    commandBuffer->begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
}

void CommandBufferOneTime::apply() {
    commandBuffer->end();

    Fence fence(device);
    fence.create();

    SubmitQueue queue(Instance::get()->getGraphicsFamily(), 0, device);
    queue.addCommandBuffer(commandBuffer->getHandle());
    queue.submit(fence.getHandle());

    device->waitForFences({ fence.getHandle() });
}

void CommandBufferOneTime::blitImage(VkImage srcImage, VkImageLayout srcImageLayout, VkImage dstImage, VkImageLayout dstImageLayout, VkFilter filter) {
    commandBuffer->blitImage(srcImage, srcImageLayout, dstImage, dstImageLayout, filter);
}

void CommandBufferOneTime::copyImage(VkImage srcImage, VkImageLayout srcImageLayout, VkImage dstImage, VkImageLayout dstImageLayout) {
    commandBuffer->copyImage(srcImage, srcImageLayout, dstImage, dstImageLayout);
}

void CommandBufferOneTime::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer) {
    commandBuffer->copyBuffer(srcBuffer, dstBuffer);
}

void CommandBufferOneTime::setImageLayout(VkImage image, VkImageAspectFlags aspectMask, VkImageLayout oldImageLayout, VkImageLayout newImageLayout, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask) {
    commandBuffer->setImageLayout(image, aspectMask, oldImageLayout, newImageLayout, srcStageMask, dstStageMask);
}

void CommandBufferOneTime::addBlitRegion(VkImageBlit blitRegion) {
    commandBuffer->addBlitRegion(blitRegion);
}

void CommandBufferOneTime::addImageCopy(VkImageCopy imageCopy) {
    commandBuffer->addImageCopy(imageCopy);
}

void CommandBufferOneTime::addBufferCopy(VkBufferCopy bufferCopy) {
    commandBuffer->addBufferCopy(bufferCopy);
}
