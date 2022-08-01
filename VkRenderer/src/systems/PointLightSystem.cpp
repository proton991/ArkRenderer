#include "PointLightSystem.hpp"
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
  PointLightSystem::PointLightSystem(ArkDevice& device, VkRenderPass renderPass,
                                     VkDescriptorSetLayout globalSetLayout) : m_arkDevice(device)
  {
    CreatePipelineLayout(globalSetLayout);
    CreatePipeline(renderPass);
  }

  PointLightSystem::~PointLightSystem()
  {
    vkDestroyPipelineLayout(m_arkDevice.Device(), m_pipelineLayout, nullptr);
  }

  void PointLightSystem::Render(FrameInfo& frameInfo)
  {
    m_arkPipeline->Bind(frameInfo.commandBuffer);

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
    vkCmdDraw(frameInfo.commandBuffer, 6, 1, 0, 0);
  }


  void PointLightSystem::CreatePipelineLayout(VkDescriptorSetLayout globalSetLayout)
  {
    //VkPushConstantRange pushConstantRange{};
    //pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    //pushConstantRange.offset = 0;
    //pushConstantRange.size = sizeof(SimplePushConstantData);

    std::vector<VkDescriptorSetLayout> descriptorSetLayouts{globalSetLayout};

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
    pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
    pipelineLayoutInfo.pushConstantRangeCount = 0;
    pipelineLayoutInfo.pPushConstantRanges = nullptr;
    if (vkCreatePipelineLayout(m_arkDevice.Device(), &pipelineLayoutInfo, nullptr, &m_pipelineLayout) !=
      VK_SUCCESS)
    {
      throw std::runtime_error("failed to create pipeline layout!");
    }
  }

  void PointLightSystem::CreatePipeline(VkRenderPass renderPass)
  {
    assert(m_pipelineLayout != nullptr, "Cannot create pipeline before pipeline layout");
    PipelineConfigInfo pipelineConfig{};
    ArkPipeline::DefaultPipelineConfigInfo(pipelineConfig);
    pipelineConfig.attributeDescriptions.clear();
    pipelineConfig.bindingDescriptions.clear();
    pipelineConfig.renderPass = renderPass;
    pipelineConfig.pipelineLayout = m_pipelineLayout;
    m_arkPipeline = std::make_unique<ArkPipeline>(m_arkDevice, "shaders/point_light.vert.spv",
                                                  "shaders/point_light.frag.spv", pipelineConfig);
  }
}
