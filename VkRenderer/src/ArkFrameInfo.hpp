#pragma once
#include "ArkCamera.hpp"
#include "ArkGameObject.hpp"
#include "ArkDescriptors.hpp"

#include <vulkan/vulkan.h>



namespace Ark
{
  struct GlobalUbo
  {
    glm::mat4 projection{ 1.0f };
    glm::mat4 view{ 1.0f };
    glm::vec4 ambientLightColor{ 1.f, 1.f, 1.f, .02f };
    glm::vec3 lightPosition{ -1.f };
    alignas(16) glm::vec4 lightColor{ 1.f }; // (r, g, b, intensity)
    /*glm::vec3 lightDirection = glm::normalize(glm::vec3{3.f, 0.f, 0.f});*/
  };
  struct FrameInfo
  {
    int frameIndex;
    double frameTime;
    VkCommandBuffer commandBuffer;
    ArkCamera& camera;
    VkDescriptorSet globalDescriptorSet;
    ArkDescriptorPool& frameDescriptorPool;  // pool of descriptors that is cleared each frame
    ArkGameObject::Map& gameObjects;
  };
}
