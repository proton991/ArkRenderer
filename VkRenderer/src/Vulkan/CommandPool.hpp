#pragma once

#include "Vulkan.hpp"

namespace Vulkan
{
  class Device;

  class CommandPool final
  {
  public:
    VULKAN_NON_COPIABLE(CommandPool)
    CommandPool(const Device& device, uint32_t queueFamilyIndex, bool allowReset);
    ~CommandPool();

    const Device& Device() const { return m_device; }

  private:
    const class Device& m_device;

    VULKAN_HANDLE(VkCommandPool, m_commandPool)
  };
}
