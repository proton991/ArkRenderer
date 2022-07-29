#include "ArkRenderer.hpp"


//std
#include <array>
#include <stdexcept>

namespace Ark
{
  ArkRenderer::ArkRenderer(WindowSystem& window, ArkDevice& device) : m_window(window), m_arkDevice(device)
  {
    RecreateSwapChain();
    CreateCommandBuffers();
  }

  ArkRenderer::~ArkRenderer()
  {
    FreeCommandBuffers();
  }


  void ArkRenderer::CreateCommandBuffers()
  {
    m_commandBuffers.resize(ArkSwapChain::MAX_FRAMES_IN_FLIGHT);
    VkCommandBufferAllocateInfo allocateInfo{};
    allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocateInfo.commandPool = m_arkDevice.GetCommandPool();
    allocateInfo.commandBufferCount = static_cast<uint32_t>(m_commandBuffers.size());
    if (vkAllocateCommandBuffers(m_arkDevice.Device(), &allocateInfo, m_commandBuffers.data()) != VK_SUCCESS)
    {
      throw std::runtime_error("failed to allocate command buffers!");
    }
  }


  void ArkRenderer::RecreateSwapChain()
  {
    auto extent = m_window.GetExtent();
    while (extent.width == 0 || extent.height == 0)
    {
      extent = m_window.GetExtent();
      glfwWaitEvents();
    }
    // wait until the swap chain is no longer being used
    vkDeviceWaitIdle(m_arkDevice.Device());
    if (m_arkSwapChain == nullptr)
    {
      m_arkSwapChain = std::make_unique<ArkSwapChain>(m_arkDevice, extent);
    }
    else
    {
      std::shared_ptr<ArkSwapChain> oldSwapChain = std::move(m_arkSwapChain);

      m_arkSwapChain = std::make_unique<ArkSwapChain>(m_arkDevice, extent, oldSwapChain);
      if (!oldSwapChain->CompareSwapChainFormats(*m_arkSwapChain))
      {
        throw std::runtime_error("Swap chain image(or depth) format has changed!");
      }
    }
  }

  void ArkRenderer::FreeCommandBuffers()
  {
    vkFreeCommandBuffers(m_arkDevice.Device(), m_arkDevice.GetCommandPool(),
                         static_cast<uint32_t>(m_commandBuffers.size()), m_commandBuffers.data());
    m_commandBuffers.clear();
  }


  VkCommandBuffer ArkRenderer::BeginFrame()
  {
    assert(!m_isFrameStarted && "Can't call BeginFrame() while already in progress");
    auto result = m_arkSwapChain->AcquireNextImage(&m_imageIndex);
    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
      RecreateSwapChain();
      return nullptr;
    }
    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
      throw std::runtime_error("failed to acquire swap chain image!");
    }
    m_isFrameStarted = true;
    auto commandBuffer = GetCurrentCommandBuffer();
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
    {
      throw std::runtime_error("failed to begin recording command buffers!");
    }
    return commandBuffer;
  }

  void ArkRenderer::EndFrame()
  {
    assert(m_isFrameStarted && "Can't call EndFrame() while frame is not in progress");
    auto commandBuffer = GetCurrentCommandBuffer();
    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
    {
      throw std::runtime_error("failed to record command buffer!");
    }
    auto result = m_arkSwapChain->SubmitCommandBuffers(&commandBuffer, &m_imageIndex);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_window.WasWindowResized())
    {
      m_window.ResetWindowResizedFlag();
      RecreateSwapChain();
    }
    else if (result != VK_SUCCESS)
    {
      throw std::runtime_error("failed to represent swap chain image!");
    }
    m_isFrameStarted = false;
    m_frameIndex = (m_frameIndex + 1) % ArkSwapChain::MAX_FRAMES_IN_FLIGHT;
  }

  void ArkRenderer::BeginSwapChainRenderPass(VkCommandBuffer commandBuffer)
  {
    assert(m_isFrameStarted && "Can't call BeginSwapChainRenderPass() while frame not in progress");
    assert(
      commandBuffer == GetCurrentCommandBuffer() &&
      "Can't begin render pass on command buffer from a different frame");
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = m_arkSwapChain->GetRenderPass();
    renderPassInfo.framebuffer = m_arkSwapChain->GetFrameBuffer(m_imageIndex);
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = m_arkSwapChain->GetSwapChainExtent();

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = {0.01f, 0.01f, 0.01f, 1.0f};
    clearValues[1].depthStencil = {1.0f, 0};
    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(m_arkSwapChain->GetSwapChainExtent().width);
    viewport.height = static_cast<float>(m_arkSwapChain->GetSwapChainExtent().height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    VkRect2D scissor{{0, 0}, m_arkSwapChain->GetSwapChainExtent()};
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
  }

  void ArkRenderer::EndSwapChainRenderPass(VkCommandBuffer commandBuffer)
  {
    assert(m_isFrameStarted && "Can't call EndSwapChainRenderPass() while not in progress");
    assert(
      commandBuffer == GetCurrentCommandBuffer() &&
      "Can't end render pass on command buffer from a different frame");
    vkCmdEndRenderPass(commandBuffer);
  }
}
