#pragma once
#include "Base/Handle.h"
#include <vector>

namespace Vulkan {

#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif

class Instance : public Handle<VkInstance> {

public:
    Instance();
    ~Instance();
    void create() override;
    void destroy() override;
    const std::vector<VkLayerProperties>& getLayers() const { return layers; }
    void setEnabledLayers(const std::vector<const char*> enabledLayers);
    void dumpLayers();

    uint32_t getExtensionCount() const { return extensions.size(); }
    VkExtensionProperties getExtension(int i) const { return extensions.at(i); }
    void setEnabledExtensions(const std::vector<const char*> enabledExtensions);
    void dumpExtensions();

private:
    VkInstanceCreateInfo createInfo = {};
    VkApplicationInfo applicationInfo = {};
    std::vector<VkLayerProperties> layers;
    std::vector<const char*> enabledLayers;
    std::vector<VkExtensionProperties> extensions;
    std::vector<const char*> enabledExtensions;
};

} // Vulkan
