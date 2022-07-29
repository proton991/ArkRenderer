#pragma once

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
    SimpleRenderSystem(ArkDevice& device, VkRenderPass renderPass);
    ~SimpleRenderSystem();

    SimpleRenderSystem(const SimpleRenderSystem&) = delete;
    SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;
    void RenderGameObjects(VkCommandBuffer commandBuffer, std::vector<ArkGameObject>& gameObjects,
                           const ArkCamera& camera);

  private:
    void CreatePipelineLayout();
    void CreatePipeline(VkRenderPass renderPass);

    ArkDevice& m_arkDevice;
    std::unique_ptr<ArkPipeline> m_arkPipeline;
    VkPipelineLayout m_pipelineLayout;
  };
}
