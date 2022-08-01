#pragma once
#include "ArkCamera.hpp"

#include <vulkan/vulkan.h>

namespace Ark
{
  struct FrameInfo
  {
    int frameIndex;
    double frameTime;
    VkCommandBuffer commandBuffer;
    ArkCamera& camera;
    VkDescriptorSet globalDescriptorSet;
  };
}
