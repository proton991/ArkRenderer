#include "CommandBuffer.hpp"

#include "CommandPool.hpp"
#include "Device.hpp"

namespace Vulkan
{
  CommandBuffer::CommandBuffer(CommandPool& commandPool, uint32_t size)
    : m_commandPool(commandPool)
  {
    VkCommandBufferAllocateInfo allocateInfo = {};
    allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocateInfo.commandPool = m_commandPool.Handle();
    allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocateInfo.commandBufferCount = size;

    m_commandBuffers.resize(size);

    Check(vkAllocateCommandBuffers(m_commandPool.Device().Handle(), &allocateInfo, m_commandBuffers.data()), "Allocate command buffers");
  }

  CommandBuffer::~CommandBuffer()
  {
    if (!m_commandBuffers.empty())
    {
      vkFreeCommandBuffers(m_commandPool.Device().Handle(), m_commandPool.Handle(), static_cast<uint32_t>(m_commandBuffers.size()), m_commandBuffers.data());
      m_commandBuffers.clear();
    }
  }

  VkCommandBuffer CommandBuffer::Begin(const size_t i)
  {
    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_DEVICE_GROUP_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
    beginInfo.pInheritanceInfo = nullptr;
    Check(vkBeginCommandBuffer(m_commandBuffers[i], &beginInfo), "Begin recording command buffer");
    return m_commandBuffers[i];
  }

  void CommandBuffer::End(const size_t i)
  {
    Check(vkEndCommandBuffer(m_commandBuffers[i]), "Record command buffer");
  }
}
