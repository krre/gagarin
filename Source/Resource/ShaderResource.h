#pragma once
#include "Resource.h"
#include <vector>
#include <map>
#include <vulkan/vulkan.h>

class ShaderResource : public Resource {

    friend class ShaderProgram;

public:

    struct Binding {
        int set;
        int binding;
        VkDescriptorType descriptorType;
    };

    struct Location {
        int location;
        std::string variableType;
        std::string valueType;
        int vectorCount;
    };

    ShaderResource();
    void load(const std::string& path) override;
    VkShaderStageFlagBits getStage() const { return stage; }
    const uint32_t* getCodeData() const { return code.data(); }
    size_t getCodeSize() const { return code.size(); }
    static VkFormat getFormat(Location* input);

    void dumpBindings();
    void dumpInputs();

private:
    VkShaderStageFlagBits stage;
    std::vector<uint32_t> code;
    std::map<std::string, Location> locations;
    std::map<std::string, Binding> bindings;

    void parse();
};
