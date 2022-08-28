#pragma once

#include "ArkDevice.hpp"

// libs
#include <vulkan/vulkan.h>

// std
#include <memory>
#include <string>

namespace Ark
{
  class Texture
  {
  public:
    Texture(ArkDevice& device, const std::string& textureFilepath);
    Texture(
      ArkDevice& device,
      VkFormat format,
      VkExtent3D extent,
      VkImageUsageFlags usage,
      VkSampleCountFlagBits sampleCount);
    ~Texture();

    // delete copy constructors
    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    VkImageView ImageView() const { return m_textureImageView; }
    VkSampler Sampler() const { return m_textureSampler; }
    VkImage GetImage() const { return m_textureImage; }
    VkImageView GetImageView() const { return m_textureImageView; }
    VkDescriptorImageInfo GetImageInfo() const { return m_descriptor; }
    VkImageLayout GetImageLayout() const { return m_textureLayout; }
    VkExtent3D GetExtent() const { return m_extent; }
    VkFormat GetFormat() const { return m_format; }

    void UpdateDescriptor();
    void TransitionLayout(
      VkCommandBuffer commandBuffer, VkImageLayout oldLayout, VkImageLayout newLayout);

    static std::unique_ptr<Texture> CreateTextureFromFile(
      ArkDevice& device, const std::string& filepath);

  private:
    void CreateTextureImage(const std::string& filepath);
    void CreateTextureImageView(VkImageViewType viewType);
    void CreateTextureSampler();

    VkDescriptorImageInfo m_descriptor{};

    ArkDevice& m_arkDevice;
    VkImage m_textureImage = VK_NULL_HANDLE;
    VkDeviceMemory m_textureImageMemory = VK_NULL_HANDLE;
    VkImageView m_textureImageView = VK_NULL_HANDLE;
    VkSampler m_textureSampler = VK_NULL_HANDLE;
    VkFormat m_format;
    VkImageLayout m_textureLayout;
    uint32_t m_mipLevels{1};
    uint32_t m_layerCount{1};
    VkExtent3D m_extent{};
  };
} // namespace lve
