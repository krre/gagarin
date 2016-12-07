#pragma once
#include "../../../Core/Common.h"
#include <vulkan/vulkan.h>
#include <string>
#include <map>

namespace Vulkan {

class Resulter {

public:
    Resulter() = default;
    std::string getResultDescription() const;
    VkResult getResult() const { return result; }
    std::string resultToString(VkResult result) const;

protected:
    VkResult checkError(VkResult result, const char* message);

    VkResult result;
};

} // Vulkan