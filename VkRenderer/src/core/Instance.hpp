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
    void Create();
    ~Instance();
    [[nodiscard]] VkInstance Get() const { return m_instance; }
    std::vector<const char*> m_requiredExtensions;
  private:
    const std::vector<const char*> validationLayers = {"VK_LAYER_KHRONOS_validation"};
    void GetRequiredInstanceExtensions();
    void CheckExtensionsSupport();
    bool CheckValidationLayerSupport();
    void SetupDebugMessenger();
    VkInstance m_instance = VK_NULL_HANDLE;
    std::vector<const char*> m_supportedExtensions;
    std::vector<VkPhysicalDevice> m_physicalDevices;
    bool m_enableValidationLayers;
    VkDebugUtilsMessengerEXT m_debugMessenger;
  };
}
