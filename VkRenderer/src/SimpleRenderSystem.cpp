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
    glm::mat4 modelMatrix{1.0f};
    glm::mat4 normalMatrix{1.0f};
  };

  SimpleRenderSystem::SimpleRenderSystem(ArkDevice& device, VkRenderPass renderPass,
                                         VkDescriptorSetLayout globalSetLayout) : m_arkDevice(device)
  {
    CreatePipelineLayout(globalSetLayout);
    CreatePipeline(renderPass);
  }

  SimpleRenderSystem::~SimpleRenderSystem()
  {
    vkDestroyPipelineLayout(m_arkDevice.Device(), m_pipelineLayout, nullptr);
  }

  void SimpleRenderSystem::RenderGameObjects(FrameInfo& frameInfo, std::vector<ArkGameObject>& gameObjects)
  {
    m_arkPipeline->Bind(frameInfo.commandBuffer);

    auto projectionView = frameInfo.camera.GetProjMatrix() * frameInfo.camera.GetViewMatrix();
    // only need to bind once!
    vkCmdBindDescriptorSets(
      frameInfo.commandBuffer,
      VK_PIPELINE_BIND_POINT_GRAPHICS,
      m_pipelineLayout,
      0,
      1,
      &frameInfo.globalDescriptorSet,
      0,
      nullptr
    );
    for (auto& obj : gameObjects)
    {
      SimplePushConstantData push{};
      push.modelMatrix = obj.m_transform.Mat4();;
      push.normalMatrix = obj.m_transform.NormalMat();
      vkCmdPushConstants(frameInfo.commandBuffer, m_pipelineLayout,
                         VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0,
                         sizeof(SimplePushConstantData), &push);
      obj.m_model->Bind(frameInfo.commandBuffer);
      obj.m_model->Draw(frameInfo.commandBuffer);
    }
  }


  void SimpleRenderSystem::CreatePipelineLayout(VkDescriptorSetLayout globalSetLayout)
  {
    VkPushConstantRange pushConstantRange{};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(SimplePushConstantData);

    std::vector<VkDescriptorSetLayout> descriptorSetLayouts{globalSetLayout};

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
    pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
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
