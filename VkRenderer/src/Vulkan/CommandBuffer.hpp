#pragma once

#include <vector>

#include "Vulkan.hpp"

namespace Vulkan
{
  class CommandPool;

  class CommandBuffer final
  {
  public:
    VULKAN_NON_COPIABLE(CommandBuffer)
    CommandBuffer(CommandPool& commandPool, uint32_t size);
    ~CommandBuffer();

    uint32_t Size() const { return static_cast<uint32_t>(m_commandBuffers.size()); }
    VkCommandBuffer& operator[](const size_t i) { return m_commandBuffers[i]; }

    VkCommandBuffer Begin(size_t i);
    void End(size_t i);

  private:
    const CommandPool& m_commandPool;
    std::vector<VkCommandBuffer> m_commandBuffers;
  };
}
