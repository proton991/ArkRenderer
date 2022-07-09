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
		glm::mat2 transform{1.0f};
		glm::vec2 offset;
		alignas(16) glm::vec3 color;
	};

	SimpleRenderSystem::SimpleRenderSystem(ArkDevice& device,
	                                       VkRenderPass renderPass) :
		m_arkDevice(device)
	{
		CreatePipelineLayout();
		CreatePipeline(renderPass);
	}

	SimpleRenderSystem::~SimpleRenderSystem()
	{
		vkDestroyPipelineLayout(m_arkDevice.Device(), m_pipelineLayout,
		                        nullptr);
	}

	void SimpleRenderSystem::RenderGameObjects(VkCommandBuffer commandBuffer,
	                                           std::vector<ArkGameObject>&
	                                           gameObjects)
	{
		m_arkPipeline->Bind(commandBuffer);
		for (auto& obj : gameObjects)
		{
			obj.m_transform2d.rotation = glm::mod(
				obj.m_transform2d.rotation + 0.1f, 360.0f);
			SimplePushConstantData push{};
			push.offset = obj.m_transform2d.translation;
			push.color = obj.m_color;
			push.transform = obj.m_transform2d.Mat2();
			vkCmdPushConstants(commandBuffer, m_pipelineLayout,
			                   VK_SHADER_STAGE_VERTEX_BIT |
			                   VK_SHADER_STAGE_FRAGMENT_BIT, 0,
			                   sizeof(SimplePushConstantData), &push);
			obj.m_model->Bind(commandBuffer);
			obj.m_model->Draw(commandBuffer);
		}
	}


	void SimpleRenderSystem::CreatePipelineLayout()
	{
		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT |
			VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(SimplePushConstantData);

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType =
			VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pSetLayouts = nullptr;
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
		if (vkCreatePipelineLayout(m_arkDevice.Device(), &pipelineLayoutInfo,
		                           nullptr, &m_pipelineLayout) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create pipeline layout!");
		}
	}

	void SimpleRenderSystem::CreatePipeline(VkRenderPass renderPass)
	{
		assert(m_pipelineLayout != nullptr,
		       "Cannot create pipeline before pipeline layout");
		PipelineConfigInfo pipelineConfig{};
		ArkPipeline::DefaultPipelineConfigInfo(
			pipelineConfig);
		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = m_pipelineLayout;
		m_arkPipeline = std::make_unique<ArkPipeline>(
			m_arkDevice, "shaders/triangle.vert.spv",
			"shaders/triangle.frag.spv", pipelineConfig);
	}
}
