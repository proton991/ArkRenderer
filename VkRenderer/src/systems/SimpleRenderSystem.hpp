#pragma once

#include "ArkFrameInfo.hpp"
#include "ArkCamera.hpp"
#include "ArkPipleline.hpp"
#include "ArkGameObject.hpp"
#include "ArkDevice.hpp"
#include <memory>

namespace Ark
{
  class SimpleRenderSystem
  {
  public:
    SimpleRenderSystem(ArkDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
    ~SimpleRenderSystem();

    SimpleRenderSystem(const SimpleRenderSystem&) = delete;
    SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;
    void RenderGameObjects(FrameInfo& frameInfo);

  private:
    void CreatePipelineLayout(VkDescriptorSetLayout globalSetLayout);
    void CreatePipeline(VkRenderPass renderPass);

    ArkDevice& m_arkDevice;
    std::unique_ptr<ArkPipeline> m_arkPipeline;
    VkPipelineLayout m_pipelineLayout;
  };
}
