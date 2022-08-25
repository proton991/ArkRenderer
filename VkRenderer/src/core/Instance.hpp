#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>
#include <vector>
#include <vulkan/vulkan.h>
#include "Utils.hpp"

namespace Ark
{
  class Instance
  {
  public:
    NON_COPYABLE(Instance)
    Instance(bool enableValidationLayers = true);
    ~Instance();
    [[nodiscard]] VkInstance Get() const { return m_instance; }
    std::vector<std::string> m_supportedInstanceExtensions;
  private:
    const std::vector<const char*> validationLayers = {"VK_LAYER_KHRONOS_validation"};
    std::vector<const char*> GetRequiredInstanceExtensions() const;
    void HasGlfwRequiredInstanceExtensions() const;
    void SetupDebugMessenger();
    VkInstance m_instance{};
    std::vector<VkPhysicalDevice> m_physicalDevices;
    bool m_enableValidationLayers;
    VkDebugUtilsMessengerEXT m_debugMessenger;
  };
}
