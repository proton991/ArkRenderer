#pragma once

#include "Instance.hpp"

#include <string>
#include <vector>
#include <vulkan/vulkan.h>
namespace Ark
{
  struct SwapChainSupportDetails
  {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
  };

  struct QueueFamilyIndices
  {
    uint32_t graphicsFamily;
    uint32_t presentFamily;
    bool graphicsFamilyHasValue = false;
    bool presentFamilyHasValue = false;

    bool IsComplete()
    {
      return graphicsFamilyHasValue && presentFamilyHasValue;
    }
  };
  class Device
  {
  public:
    Device();
  private:
    /**
     * \brief Physical Device
     */
    VkPhysicalDevice m_physicalDevice;
    /**
     * \brief Logical Device
     */
    VkDevice m_logicalDevice;

    /**
     * \brief Physical device properties
     */
    VkPhysicalDeviceProperties m_properties;

    /**
     * \brief Physical device features
     */
    VkPhysicalDeviceFeatures m_features;
    /**
     * \brief Enabled physical device features
     */
    VkPhysicalDeviceFeatures m_enabledFeatures{};

    VkPhysicalDeviceMemoryProperties m_memoryProperties;

    std::vector<std::string> m_supportedExtensions;

    VkCommandPool m_commandPool;
  };
}
