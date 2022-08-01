#pragma once

#include <memory>
#include <unordered_map>

#include "ArkDevice.hpp"

namespace Ark
{
  class ArkDescriptorSetLayout
  {
  public:
    class Builder
    {
    public:
      Builder(ArkDevice& arkDevice) : m_arkDevice{arkDevice}
      {
      }

      Builder& AddBinding(
        uint32_t binding,
        VkDescriptorType descriptorType,
        VkShaderStageFlags stageFlags,
        uint32_t count = 1);
      std::unique_ptr<ArkDescriptorSetLayout> Build() const;

    private:
      ArkDevice& m_arkDevice;
      std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> m_bindings{};
    };

    ArkDescriptorSetLayout(
      ArkDevice& arkDevice, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
    ~ArkDescriptorSetLayout();
    ArkDescriptorSetLayout(const ArkDescriptorSetLayout&) = delete;
    ArkDescriptorSetLayout& operator=(const ArkDescriptorSetLayout&) = delete;

    VkDescriptorSetLayout GetDescriptorSetLayout() const { return m_descriptorSetLayout; }

  private:
    ArkDevice& m_arkDevice;
    VkDescriptorSetLayout m_descriptorSetLayout;
    std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> m_bindings;

    friend class ArkDescriptorWriter;
  };

  class ArkDescriptorPool
  {
  public:
    class Builder
    {
    public:
      Builder(ArkDevice& arkDevice) : m_arkDevice{arkDevice}
      {
      }

      Builder& AddPoolSize(VkDescriptorType descriptorType, uint32_t count);
      Builder& SetPoolFlags(VkDescriptorPoolCreateFlags flags);
      Builder& SetMaxSets(uint32_t count);
      std::unique_ptr<ArkDescriptorPool> Build() const;

    private:
      ArkDevice& m_arkDevice;
      std::vector<VkDescriptorPoolSize> m_poolSizes{};
      uint32_t m_maxSets = 1000;
      VkDescriptorPoolCreateFlags m_poolFlags = 0;
    };

    ArkDescriptorPool(
      ArkDevice& arkDevice,
      uint32_t maxSets,
      VkDescriptorPoolCreateFlags poolFlags,
      const std::vector<VkDescriptorPoolSize>& poolSizes);
    ~ArkDescriptorPool();
    ArkDescriptorPool(const ArkDescriptorPool&) = delete;
    ArkDescriptorPool& operator=(const ArkDescriptorPool&) = delete;

    bool AllocateDescriptor(
      const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const;

    void FreeDescriptors(std::vector<VkDescriptorSet>& descriptors) const;

    void ResetPool();

  private:
    ArkDevice& m_arkDevice;
    VkDescriptorPool m_descriptorPool;

    friend class ArkDescriptorWriter;
  };

  class ArkDescriptorWriter
  {
  public:
    ArkDescriptorWriter(ArkDescriptorSetLayout& setLayout, ArkDescriptorPool& pool);

    ArkDescriptorWriter& WriteBuffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo);
    ArkDescriptorWriter& WriteImage(uint32_t binding, VkDescriptorImageInfo* imageInfo);

    bool Build(VkDescriptorSet& set);
    void Overwrite(VkDescriptorSet& set);

  private:
    ArkDescriptorSetLayout& m_setLayout;
    ArkDescriptorPool& m_pool;
    std::vector<VkWriteDescriptorSet> m_writes;
  };
}
