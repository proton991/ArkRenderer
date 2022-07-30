#include "SimpleRenderSystem.hpp"
//libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

//std
#include <array>
#include <stdexcept>

namespace Ark
{
  struct SimplePushConstantData
  {
    glm::mat4 transform{1.0f};
    glm::mat4 normalMatrix{1.0f};
  };

  SimpleRenderSystem::SimpleRenderSystem(ArkDevice& device, VkRenderPass renderPass) : m_arkDevice(device)
  {
    CreatePipelineLayout();
    CreatePipeline(renderPass);
  }

  SimpleRenderSystem::~SimpleRenderSystem()
  {
    vkDestroyPipelineLayout(m_arkDevice.Device(), m_pipelineLayout, nullptr);
  }

  void SimpleRenderSystem::RenderGameObjects(VkCommandBuffer commandBuffer,
                                             std::vector<ArkGameObject>& gameObjects, const ArkCamera& camera)
  {
    m_arkPipeline->Bind(commandBuffer);

    auto projectionView = camera.GetProjMatrix() * camera.GetViewMatrix();
    for (auto& obj : gameObjects)
    {
      SimplePushConstantData push{};
      auto modelMatrix = obj.m_transform.Mat4();
      push.transform = projectionView * modelMatrix;
      push.normalMatrix = obj.m_transform.NormalMat();
      vkCmdPushConstants(commandBuffer, m_pipelineLayout,
                         VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0,
                         sizeof(SimplePushConstantData), &push);
      obj.m_model->Bind(commandBuffer);
      obj.m_model->Draw(commandBuffer);
    }
  }


  void SimpleRenderSystem::CreatePipelineLayout()
  {
    VkPushConstantRange pushConstantRange{};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(SimplePushConstantData);

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0;
    pipelineLayoutInfo.pSetLayouts = nullptr;
    pipelineLayoutInfo.pushConstantRangeCount = 1;
    pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
    if (vkCreatePipelineLayout(m_arkDevice.Device(), &pipelineLayoutInfo, nullptr, &m_pipelineLayout) !=
      VK_SUCCESS)
    {
      throw std::runtime_error("failed to create pipeline layout!");
    }
  }

  void SimpleRenderSystem::CreatePipeline(VkRenderPass renderPass)
  {
    assert(m_pipelineLayout != nullptr, "Cannot create pipeline before pipeline layout");
    PipelineConfigInfo pipelineConfig{};
    ArkPipeline::DefaultPipelineConfigInfo(pipelineConfig);
    pipelineConfig.renderPass = renderPass;
    pipelineConfig.pipelineLayout = m_pipelineLayout;
    m_arkPipeline = std::make_unique<ArkPipeline>(m_arkDevice, "shaders/triangle.vert.spv",
                                                  "shaders/triangle.frag.spv", pipelineConfig);
  }
}
