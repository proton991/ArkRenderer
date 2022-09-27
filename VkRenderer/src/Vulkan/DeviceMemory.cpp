#include "DeviceMemory.hpp"

#include <stdexcept>

#include "Device.hpp"

namespace Vulkan
{
  DeviceMemory::DeviceMemory(const Vulkan::Device& device, size_t size, uint32_t memoryTypeBits,
                             VkMemoryAllocateFlags allocateFLags, VkMemoryPropertyFlags propertyFlags)
    : m_device(device)
  {
    VkMemoryAllocateFlagsInfo flagsInfo = {};
    flagsInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO;
    flagsInfo.pNext = nullptr;
    flagsInfo.flags = allocateFLags;

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.pNext = &flagsInfo;
    allocInfo.allocationSize = size;
    allocInfo.memoryTypeIndex = FindMemoryType(memoryTypeBits, propertyFlags);

    Check(vkAllocateMemory(m_device.Handle(), &allocInfo, nullptr, &m_memory),
          "allocate memory");
  }


  DeviceMemory::DeviceMemory(DeviceMemory&& other) noexcept
    : m_device(other.m_device), m_memory(other.m_memory)
  {
    other.m_memory = nullptr;
  }

  DeviceMemory::~DeviceMemory()
  {
    if (m_memory != nullptr)
    {
      vkFreeMemory(m_device.Handle(), m_memory, nullptr);
      m_memory = nullptr;
    }
  }

  void* DeviceMemory::Map(size_t offset, size_t size)
  {
    void* data;
    Check(vkMapMemory(m_device.Handle(), m_memory, offset, size, 0, &data),
          "map memory");

    return data;
  }

  void DeviceMemory::Unmap()
  {
    vkUnmapMemory(m_device.Handle(), m_memory);
  }

  uint32_t DeviceMemory::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags propertyFlags) const
  {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(m_device.PhysicalDevice(), &memProperties);

    for (uint32_t i = 0; i != memProperties.memoryTypeCount; ++i)
    {
      if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & propertyFlags) == propertyFlags)
      {
        return i;
      }
    }
    throw std::runtime_error("Failed to find memory type!");
  }
}
