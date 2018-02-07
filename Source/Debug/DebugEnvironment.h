#pragma once
#include "Graphics/GraphicsTypes.h"
#include <json/json.hpp>

namespace Origin {

using json = nlohmann::json;

class DebugEnvironment {

public:
    DebugEnvironment();
    bool getEnable() const { return enable; }
    void setDebugScreen();
    json& getSettings() { return settings; }
    int getVulkanDevice() { return settings["vulkan"]["device"]; }

private:
    void loadValues();
    bool enable = false;
    json settings;
};

} // Origin
