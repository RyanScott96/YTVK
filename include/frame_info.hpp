#pragma once

#include "camera.hpp"

#include <vulkan/vulkan.h>

namespace YTVK
{
    struct FrameInfo
    {
        int frameIndex;
        float frameTime;
        VkCommandBuffer commandBuffer;
        Camera &camera;
    };
}