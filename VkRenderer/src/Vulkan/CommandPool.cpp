#include "CommandPool.hpp"

#include "Device.hpp"

namespace Vulkan
{
  CommandPool::CommandPool(const Vulkan::Device& device, uint32_t queueFamilyIndex, bool allowReset) : m_device(device)
  {
    VkCommandPoolCreateInfo commandPoolInfo = {};
    commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    commandPoolInfo.queueFamilyIndex = queueFamilyIndex;
    commandPoolInfo.flags = allowReset ? VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT : 0;
    Check(vkCreateCommandPool(device.Handle(), &commandPoolInfo, nullptr, &m_commandPool),
          "Create command pool");
  }

  CommandPool::~CommandPool()
  {
    if (m_commandPool != nullptr)
    {
      vkDestroyCommandPool(m_device.Handle(), m_commandPool, nullptr);
      m_commandPool = nullptr;
    }
  }
}
