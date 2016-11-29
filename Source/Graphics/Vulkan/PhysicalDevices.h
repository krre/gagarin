#pragma once
#include "VkObject.h"
#include "Instance.h"
#include <vector>
#include <map>

namespace Vulkan {

class PhysicalDevices : public VkObject {

public:
    PhysicalDevices(const Instance* instance);
    uint32_t getCount() const { return devices.size(); }
    VkPhysicalDevice getDevice(size_t i) const { return devices.at(i); }
    VkPhysicalDeviceProperties getProperties(VkPhysicalDevice device) { return properties[device]; }
    VkPhysicalDeviceFeatures getFeatures(VkPhysicalDevice device) { return features[device]; }
    std::vector<VkQueueFamilyProperties> getQueueFamilyProperties(VkPhysicalDevice device) { return queueFamilyProperties[device]; }
    int findQueue(VkPhysicalDevice device, VkQueueFlags flags);
    void dumpDevices();

private:
    const Instance* instance;
    std::vector<VkPhysicalDevice> devices;
    std::map<VkPhysicalDevice, VkPhysicalDeviceProperties> properties;
    std::map<VkPhysicalDevice, VkPhysicalDeviceFeatures> features;
    std::map<VkPhysicalDevice, std::vector<VkQueueFamilyProperties>> queueFamilyProperties;
};

} // Vulkan
