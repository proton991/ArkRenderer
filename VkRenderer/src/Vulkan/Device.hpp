#pragma once

#include "Vulkan.hpp"

#include <vector>

#include "DebugUtils.hpp"

namespace Vulkan
{
  class Surface;

  class Device final
  {
  public:
    VULKAN_NON_COPIABLE(Device);
    Device(
      VkPhysicalDevice physicalDevice,
      const Surface& surface,
      const std::vector<const char*>& requiredExtensions,
      const VkPhysicalDeviceFeatures& deviceFeatures,
      const void* nextDeviceFeatures);

    ~Device();

    void WaitIdle() const;

    [[nodiscard]] const Surface& Surface() const
    {
      return m_surface;
    }

    [[nodiscard]] VkPhysicalDevice PhysicalDevice() const
    {
      return m_physicalDevice;
    }

    [[nodiscard]] uint32_t GraphicsQueueFamilyIndex() const
    {
      return m_graphicsQueueFamilyIndex;
    }

    [[nodiscard]] uint32_t ComputeQueueFamilyIndex() const
    {
      return m_computeQueueFamilyIndex;
    }

    [[nodiscard]] uint32_t PresentQueueFamilyIndex() const
    {
      return m_presentQueueFamilyIndex;
    }

    [[nodiscard]] uint32_t TransferQueueFamilyIndex() const
    {
      return m_transferQueueFamilyIndex;
    }

    [[nodiscard]] VkQueue GraphicsQueue() const
    {
      return m_graphicsQueue;
    }

    [[nodiscard]] VkQueue ComputeQueue() const
    {
      return m_computeQueue;
    }

    [[nodiscard]] VkQueue PresentQueue() const
    {
      return m_presentQueue;
    }

    [[nodiscard]] VkQueue TransferQueue() const
    {
      return m_transferQueue;
    }

  private:
    VkPhysicalDevice m_physicalDevice;
    const class Surface& m_surface;

    VULKAN_HANDLE(VkDevice, m_device)

    class DebugUtils m_debugUtils;
    uint32_t m_graphicsQueueFamilyIndex{};
    uint32_t m_computeQueueFamilyIndex{};
    uint32_t m_presentQueueFamilyIndex{};
    uint32_t m_transferQueueFamilyIndex{};

    VkQueue m_graphicsQueue{};
    VkQueue m_computeQueue{};
    VkQueue m_presentQueue{};
    VkQueue m_transferQueue{};

    void CheckRequiredExtensions(VkPhysicalDevice physicalDevice,
                                 const std::vector<const char*>& requiredExtensions) const;
  };
}
