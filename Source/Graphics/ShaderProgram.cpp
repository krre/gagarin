#include "ShaderProgram.h"
#include "Vulkan/Pipeline/PipelineLayout.h"
#include "Vulkan/Pipeline/GraphicsPipeline.h"
#include "Vulkan/Descriptor/DescriptorPool.h"
#include "Vulkan/Descriptor/DescriptorSetLayout.h"
#include "Vulkan/Descriptor/DescriptorSets.h"
#include "Resource/ResourceManager.h"
#include "Resource/ShaderResource.h"
#include "Core/Utils.h"
#include "Vulkan/Instance.h"
#include "Vulkan/Swapchain.h"
#include "Vulkan/Buffer.h"
#include "Vulkan/Surface.h"

using namespace Vulkan;

ShaderProgram::ShaderProgram() {
    descriptorPool = std::unique_ptr<DescriptorPool>(new DescriptorPool);
    descriptorSets = std::unique_ptr<DescriptorSets>(new DescriptorSets(descriptorPool.get()));

    graphicsPipeline = std::unique_ptr<GraphicsPipeline>(new GraphicsPipeline());
    graphicsPipeline->setExtent(Instance::get()->getSurface()->getCapabilities().currentExtent);
    graphicsPipeline->setRenderPass(Instance::get()->getSurface()->getSwapchain()->getRenderPass()->getHandle());

    pipelineLayout = std::unique_ptr<PipelineLayout>(new PipelineLayout);
    descriptorSetLayout = std::unique_ptr<DescriptorSetLayout>(new DescriptorSetLayout);
}

ShaderProgram::~ShaderProgram() {
    descriptorSets->destroy();
    descriptorPool->destroy();
}

void ShaderProgram::addShader(const std::string& path) {
    ShaderResource* shaderResource = ResourceManager::get()->load<ShaderResource>(path);
    shaderResources.push_back(shaderResource);
    graphicsPipeline->addShaderCode(shaderResource->getStage(), shaderResource->getCodeSize() * sizeof(uint32_t), shaderResource->getCodeData());
}

void ShaderProgram::createPipeline() {
    std::map<VkDescriptorType, uint32_t> descriptorsTypes;

    for (auto& shaderResource : shaderResources) {
//        shaderResource->dumpDescriptors();
//        shaderResource->dumpInputs();

        for (auto& descriptorIt : shaderResource->descriptors) {
            ShaderResource::Descriptor* descriptor = &descriptorIt.second;
            if (descriptorsTypes.find(descriptor->descriptorType) == descriptorsTypes.end()) {
                descriptorsTypes[descriptor->descriptorType] = 1;
            } else {
                descriptorsTypes[descriptor->descriptorType]++;
            }

            VkDescriptorSetLayoutBinding layoutBinding = {};
            layoutBinding.binding = descriptor->binding;
            layoutBinding.descriptorCount = 1;
            layoutBinding.descriptorType = descriptor->descriptorType;
            layoutBinding.stageFlags = shaderResource->stage;

            descriptorSetLayout->addLayoutBinding(layoutBinding);

            VkWriteDescriptorSet descriptorWrite = {};
            descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrite.dstBinding = layoutBinding.binding;
            descriptorWrite.dstArrayElement = 0;
            descriptorWrite.descriptorType = layoutBinding.descriptorType;
            descriptorWrite.descriptorCount = layoutBinding.descriptorCount;

            const auto& bufferIt = bufferInfos.find(descriptorIt.first);
            if (bufferIt != bufferInfos.end()) {
                VkBufferUsageFlagBits usage;
                if (descriptor->descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER) {
                    usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
                } else if (descriptor->descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER) {
                    usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
                }
                BufferInfo* bufferInfo = &bufferIt->second;
                std::shared_ptr<Buffer> buffer = std::make_shared<Buffer>(usage, bufferInfo->size, false);
                buffer->create();
                bufferInfo->buffer = buffer;
                descriptorWrite.pBufferInfo = buffer->getDescriptorInfo();
                descriptorSets->addWriteDescriptorSet(descriptorWrite);
            }

            const auto& imageIt = imageInfos.find(descriptorIt.first);
            if (imageIt != imageInfos.end()) {
                descriptorWrite.pImageInfo = &imageIt->second;
                descriptorSets->addWriteDescriptorSet(descriptorWrite);
            }
        }

        for (auto& inputIt : shaderResource->inputs) {
            const std::string& name = inputIt.first;
            const auto& inputInfoIt = inputInfos.find(name);
            if (inputInfoIt != inputInfos.end()) {
                ShaderResource::Input* input = &inputIt.second;
                inputInfos.at(name).location = input->location;
                inputInfos.at(name).format = ShaderResource::getFormat(input);
                graphicsPipeline->addVertexAttributeDescription(inputInfos.at(name));
            }
        }
    }

    descriptorSetLayout->create();
    descriptorSets->addDescriptorSetLayout(descriptorSetLayout->getHandle());
    pipelineLayout->addDescriptorSetLayout(descriptorSetLayout->getHandle());
    pipelineLayout->create();
    graphicsPipeline->setPipelineLayout(pipelineLayout->getHandle());

    // Descriptor pool
    for (auto& it : descriptorsTypes) {
        descriptorPool->addPoolSize(it.first, it.second);
    }

    descriptorPool->create();
    descriptorSets->allocate();
    descriptorSets->writeDescriptors();

    graphicsPipeline->create();
}

int ShaderProgram::createVertexInputBindingDescription(uint32_t stride, VkVertexInputRate inputRate) {
    VkVertexInputBindingDescription bindingDescription = {};
    bindingDescription.binding = vertexBindingCount++;
    bindingDescription.inputRate = inputRate;
    bindingDescription.stride = stride;
    graphicsPipeline->addVertexBindingDescription(bindingDescription);
    return bindingDescription.binding;
}

void ShaderProgram::linkUniform(const std::string& name, uint32_t size, void* uniform) {
    BufferInfo linkInfo = {};
    linkInfo.size = size;
    linkInfo.uniform = uniform;
    bufferInfos[name] = linkInfo;
}

void ShaderProgram::linkImage(const std::string& name, VkDescriptorImageInfo descriptorImageInfo) {
    imageInfos[name] = descriptorImageInfo;
}

void ShaderProgram::linkInput(const std::string& name, uint32_t binding, uint32_t offset) {
    inputInfos[name].binding = binding;
    inputInfos[name].offset = offset;
}

void ShaderProgram::writeUniform(const std::string& name, VkDeviceSize offset, VkDeviceSize size, void* data) {
    bufferInfos.at(name).buffer->write(data != nullptr ? data : bufferInfos.at(name).uniform, size ? size : bufferInfos.at(name).size, offset);
}

void ShaderProgram::readUniform(const std::string& name, VkDeviceSize offset, VkDeviceSize size, void* data) {
    bufferInfos.at(name).buffer->read(data != nullptr ? data : bufferInfos.at(name).uniform, size ? size : bufferInfos.at(name).size, offset);
}
