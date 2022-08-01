#include "ArkDescriptors.hpp"

// std
#include <cassert>
#include <stdexcept>

namespace Ark
{
  // *************** Descriptor Set Layout Builder *********************
  ArkDescriptorSetLayout::Builder& ArkDescriptorSetLayout::Builder::AddBinding(
    uint32_t binding,
    VkDescriptorType descriptorType,
    VkShaderStageFlags stageFlags,
    uint32_t count)
  {
    assert(m_bindings.count(binding) == 0 && "Binding already in use");
    VkDescriptorSetLayoutBinding layoutBinding{};
    layoutBinding.binding = binding;
    layoutBinding.descriptorType = descriptorType;
    layoutBinding.descriptorCount = count;
    layoutBinding.stageFlags = stageFlags;
    m_bindings[binding] = layoutBinding;
    return *this;
  }

  std::unique_ptr<ArkDescriptorSetLayout> ArkDescriptorSetLayout::Builder::Build() const
  {
    return std::make_unique<ArkDescriptorSetLayout>(m_arkDevice, m_bindings);
  }

  // *************** Descriptor Set Layout *********************
  ArkDescriptorSetLayout::ArkDescriptorSetLayout(
    ArkDevice& arkDevice, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings)
    : m_arkDevice{arkDevice}, m_bindings{bindings}
  {
    std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings{};
    for (auto& kv : bindings)
    {
      setLayoutBindings.push_back(kv.second);
    }

    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo{};
    descriptorSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorSetLayoutInfo.bindingCount = static_cast<uint32_t>(setLayoutBindings.size());
    descriptorSetLayoutInfo.pBindings = setLayoutBindings.data();

    if (vkCreateDescriptorSetLayout(
      arkDevice.Device(),
      &descriptorSetLayoutInfo,
      nullptr,
      &m_descriptorSetLayout) != VK_SUCCESS)
    {
      throw std::runtime_error("failed to create descriptor set layout!");
    }
  }

  ArkDescriptorSetLayout::~ArkDescriptorSetLayout()
  {
    vkDestroyDescriptorSetLayout(m_arkDevice.Device(), m_descriptorSetLayout, nullptr);
  }

  // *************** Descriptor Pool Builder *********************

  ArkDescriptorPool::Builder& ArkDescriptorPool::Builder::AddPoolSize(
    VkDescriptorType descriptorType, uint32_t count)
  {
    m_poolSizes.push_back({descriptorType, count});
    return *this;
  }

  ArkDescriptorPool::Builder& ArkDescriptorPool::Builder::SetPoolFlags(
    VkDescriptorPoolCreateFlags flags)
  {
    m_poolFlags = flags;
    return *this;
  }

  ArkDescriptorPool::Builder& ArkDescriptorPool::Builder::SetMaxSets(uint32_t count)
  {
    m_maxSets = count;
    return *this;
  }

  std::unique_ptr<ArkDescriptorPool> ArkDescriptorPool::Builder::Build() const
  {
    return std::make_unique<ArkDescriptorPool>(m_arkDevice, m_maxSets, m_poolFlags, m_poolSizes);
  }

  // *************** Descriptor Pool *********************

  ArkDescriptorPool::ArkDescriptorPool(
    ArkDevice& arkDevice,
    uint32_t maxSets,
    VkDescriptorPoolCreateFlags poolFlags,
    const std::vector<VkDescriptorPoolSize>& poolSizes)
    : m_arkDevice{arkDevice}
  {
    VkDescriptorPoolCreateInfo descriptorPoolInfo{};
    descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    descriptorPoolInfo.pPoolSizes = poolSizes.data();
    descriptorPoolInfo.maxSets = maxSets;
    descriptorPoolInfo.flags = poolFlags;

    if (vkCreateDescriptorPool(arkDevice.Device(), &descriptorPoolInfo, nullptr, &m_descriptorPool) !=
      VK_SUCCESS)
    {
      throw std::runtime_error("failed to create descriptor pool!");
    }
  }

  ArkDescriptorPool::~ArkDescriptorPool()
  {
    vkDestroyDescriptorPool(m_arkDevice.Device(), m_descriptorPool, nullptr);
  }

  bool ArkDescriptorPool::AllocateDescriptor(
    const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const
  {
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = m_descriptorPool;
    allocInfo.pSetLayouts = &descriptorSetLayout;
    allocInfo.descriptorSetCount = 1;

    // Might want to create a "DescriptorPoolManager" class that handles this case, and builds
    // a new pool whenever an old pool fills up. But this is beyond our current scope
    if (vkAllocateDescriptorSets(m_arkDevice.Device(), &allocInfo, &descriptor) != VK_SUCCESS)
    {
      return false;
    }
    return true;
  }

  void ArkDescriptorPool::FreeDescriptors(std::vector<VkDescriptorSet>& descriptors) const
  {
    vkFreeDescriptorSets(
      m_arkDevice.Device(),
      m_descriptorPool,
      static_cast<uint32_t>(descriptors.size()),
      descriptors.data());
  }

  void ArkDescriptorPool::ResetPool()
  {
    vkResetDescriptorPool(m_arkDevice.Device(), m_descriptorPool, 0);
  }

  // *************** Descriptor Writer *********************

  ArkDescriptorWriter::ArkDescriptorWriter(ArkDescriptorSetLayout& setLayout, ArkDescriptorPool& pool)
      : m_setLayout{ setLayout }, m_pool{ pool } {}

  ArkDescriptorWriter& ArkDescriptorWriter::WriteBuffer(
      uint32_t binding, VkDescriptorBufferInfo* bufferInfo) {
      assert(m_setLayout.m_bindings.count(binding) == 1 && "Layout does not contain specified binding");

      auto& bindingDescription = m_setLayout.m_bindings[binding];

      assert(
          bindingDescription.descriptorCount == 1 &&
          "Binding single descriptor info, but binding expects multiple");

      VkWriteDescriptorSet write{};
      write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      write.descriptorType = bindingDescription.descriptorType;
      write.dstBinding = binding;
      write.pBufferInfo = bufferInfo;
      write.descriptorCount = 1;

      m_writes.push_back(write);
      return *this;
  }

  ArkDescriptorWriter& ArkDescriptorWriter::WriteImage(
      uint32_t binding, VkDescriptorImageInfo* imageInfo) {
      assert(m_setLayout.m_bindings.count(binding) == 1 && "Layout does not contain specified binding");

      auto& bindingDescription = m_setLayout.m_bindings[binding];

      assert(
          bindingDescription.descriptorCount == 1 &&
          "Binding single descriptor info, but binding expects multiple");

      VkWriteDescriptorSet write{};
      write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      write.descriptorType = bindingDescription.descriptorType;
      write.dstBinding = binding;
      write.pImageInfo = imageInfo;
      write.descriptorCount = 1;

      m_writes.push_back(write);
      return *this;
  }

  bool ArkDescriptorWriter::Build(VkDescriptorSet& set) {
      bool success = m_pool.AllocateDescriptor(m_setLayout.GetDescriptorSetLayout(), set);
      if (!success) {
          return false;
      }
      Overwrite(set);
      return true;
  }

  void ArkDescriptorWriter::Overwrite(VkDescriptorSet& set) {
      for (auto& write : m_writes) {
          write.dstSet = set;
      }
      vkUpdateDescriptorSets(m_pool.m_arkDevice.Device(), m_writes.size(), m_writes.data(), 0, nullptr);
  }
}
