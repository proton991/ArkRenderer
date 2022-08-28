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

  void SimpleRenderSystem::RenderGameObjects(FrameInfo& frameInfo)
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
    for (auto& kv : frameInfo.gameObjects)
    {
      auto& obj = kv.second;
      if (obj.m_model == nullptr) continue;

      auto bufferInfo = obj.GetBufferInfo(frameInfo.frameIndex);
      auto imageInfo = obj.m_diffuseMap->GetImageInfo();
      VkDescriptorSet gameObjectDescriptorSet;
      ArkDescriptorWriter(*m_renderSystemLayout, frameInfo.frameDescriptorPool)
        .WriteBuffer(0, &bufferInfo)
        .WriteImage(1, &imageInfo)
        .Build(gameObjectDescriptorSet);
      vkCmdBindDescriptorSets(
        frameInfo.commandBuffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        m_pipelineLayout,
        1, // starting set (0 is the globalDescriptorSet, 1 is the set specific to this system)
        1, // set count
        &gameObjectDescriptorSet,
        0,
        nullptr);
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
    m_renderSystemLayout = ArkDescriptorSetLayout::Builder(m_arkDevice)
                           .AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                       VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT)
                           .AddBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                       VK_SHADER_STAGE_FRAGMENT_BIT)
                           .Build();
    std::vector<VkDescriptorSetLayout> descriptorSetLayouts{
      globalSetLayout, m_renderSystemLayout->GetDescriptorSetLayout()
    };

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
    m_arkPipeline = std::make_unique<ArkPipeline>(m_arkDevice, "shaders/simple.vert.spv",
                                                  "shaders/simple.frag.spv", pipelineConfig);
  }
}
