#pragma once
#include "Vulkan.hpp"
#include <vector>

namespace Vulkan
{
  class Instance final
  {
  public:
    VULKAN_NON_COPIABLE(Instance)
    Instance(const std::vector<const char*>& validationLayers, uint32_t vulkanVersion);
    ~Instance();

    [[nodiscard]] std::vector<const char*> ValidationLayers() const
    {
      return m_validationLayers;
    }

    [[nodiscard]] std::vector<VkExtensionProperties> Extensions() const
    {
      return m_extensions;
    }

    [[nodiscard]] std::vector<VkLayerProperties> Layers() const
    {
      return m_layers;
    }

    [[nodiscard]] std::vector<VkPhysicalDevice> PhysicalDevices() const
    {
      return m_physicalDevices;
    }


  private:
    [[nodiscard]] std::vector<const char*> GetRequiredExtensions() const;
    void GetSupportedExtensions();
    void GetVulkanLayers();
    void GetVulkanPhysicalDevices();

    static void CheckVulkanMinVersion(uint32_t minVersion);
    static void CheckVulkanValidationLayerSupport(const std::vector<const char*>& validationLayers);


    const std::vector<const char*> m_validationLayers;
    VULKAN_HANDLE(VkInstance, m_instance)
    std::vector<VkExtensionProperties> m_extensions;
    std::vector<VkLayerProperties> m_layers;
    std::vector<VkPhysicalDevice> m_physicalDevices;
  };
}
