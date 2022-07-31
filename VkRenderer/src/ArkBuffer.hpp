#pragma once

#include "ArkDevice.hpp"

namespace Ark {

    class ArkBuffer {
    public:
        ArkBuffer(
            ArkDevice& device,
            VkDeviceSize instanceSize,
            uint32_t instanceCount,
            VkBufferUsageFlags usageFlags,
            VkMemoryPropertyFlags memoryPropertyFlags,
            VkDeviceSize minOffsetAlignment = 1);
        ~ArkBuffer();

        ArkBuffer(const ArkBuffer&) = delete;
        ArkBuffer& operator=(const ArkBuffer&) = delete;

        VkResult Map(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
        void Unmap();

        void WriteToBuffer(void* data, VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
        VkResult Flush(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
        VkDescriptorBufferInfo DescriptorInfo(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
        VkResult Invalidate(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

        void WriteToIndex(void* data, int index);
        VkResult FlushIndex(int index);
        VkDescriptorBufferInfo DescriptorInfoForIndex(int index);
        VkResult InvalidateIndex(int index);

        VkBuffer GetBuffer() const { return m_buffer; }
        void* GetMappedMemory() const { return m_mapped; }
        uint32_t GetInstanceCount() const { return m_instanceCount; }
        VkDeviceSize GetInstanceSize() const { return m_instanceSize; }
        VkDeviceSize GetAlignmentSize() const { return m_alignmentSize; }
        VkBufferUsageFlags GetUsageFlags() const { return m_usageFlags; }
        VkMemoryPropertyFlags GetMemoryPropertyFlags() const { return m_memoryPropertyFlags; }
        VkDeviceSize GetBufferSize() const { return m_bufferSize; }

    private:
        static VkDeviceSize GetAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment);

        ArkDevice& m_arkDevice;
        void* m_mapped = nullptr;
        VkBuffer m_buffer = VK_NULL_HANDLE;
        VkDeviceMemory m_memory = VK_NULL_HANDLE;

        VkDeviceSize m_bufferSize;
        uint32_t m_instanceCount;
        VkDeviceSize m_instanceSize;
        VkDeviceSize m_alignmentSize;
        VkBufferUsageFlags m_usageFlags;
        VkMemoryPropertyFlags m_memoryPropertyFlags;
    };

}  // namespace lve

