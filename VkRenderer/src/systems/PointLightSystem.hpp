#pragma once

#include "ArkFrameInfo.hpp"
#include "ArkCamera.hpp"
#include "ArkPipleline.hpp"
#include "ArkGameObject.hpp"
#include "ArkDevice.hpp"
#include <memory>

namespace Ark
{
  class PointLightSystem
  {
  public:
    PointLightSystem(ArkDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
    ~PointLightSystem();

    PointLightSystem(const PointLightSystem&) = delete;
    PointLightSystem& operator=(const PointLightSystem&) = delete;
    void Render(FrameInfo& frameInfo);

  private:
    void CreatePipeline(VkRenderPass renderPass);
    void CreatePipelineLayout(VkDescriptorSetLayout globalSetLayout);
    ArkDevice& m_arkDevice;
    std::unique_ptr<ArkPipeline> m_arkPipeline;
    VkPipelineLayout m_pipelineLayout;
  };
}
