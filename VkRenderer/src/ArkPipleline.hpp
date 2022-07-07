#pragma once
#include <string>
#include "ArkDevice.hpp"

namespace Ark
{
	struct PipelineConfigInfo
	{
		//VkViewport viewport;
		//VkRect2D scissor;
		VkPipelineViewportStateCreateInfo viewportInfo;
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
		VkPipelineRasterizationStateCreateInfo rasterizationInfo;
		VkPipelineMultisampleStateCreateInfo multisampleInfo;
		VkPipelineColorBlendAttachmentState colorBlendAttachment;
		VkPipelineColorBlendStateCreateInfo colorBlendInfo;
		VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
		std::vector<VkDynamicState> dynamicStatesEnables;
		VkPipelineDynamicStateCreateInfo dynamicStateInfo;
		VkPipelineLayout pipelineLayout = nullptr;
		VkRenderPass renderPass = nullptr;
		uint32_t subpass = 0;
		PipelineConfigInfo() = default;
		PipelineConfigInfo(const PipelineConfigInfo&) = delete;
		PipelineConfigInfo& operator=(const PipelineConfigInfo&) = delete;
	};

	class ArkPipeline
	{
	public:
		ArkPipeline(ArkDevice& device, const std::string& vertShaderPath,
		            const std::string& fragShaderPath,
		            const PipelineConfigInfo& configInfo);

		~ArkPipeline();
		ArkPipeline(const ArkPipeline&) = delete;
		ArkPipeline& operator=(const ArkPipeline&) = delete;
		void Bind(VkCommandBuffer commandBuffer);
		static void DefaultPipelineConfigInfo(
			PipelineConfigInfo& configInfo);
	private:
		ArkDevice& m_arkDevice;
		VkPipeline m_graphicsPipeline;
		VkShaderModule m_vertexShaderModule;
		VkShaderModule m_fragShaderModule;
		void CreateGraphicsPipeline(const std::string& vertShaderPath,
		                            const std::string& fragShaderPath,
		                            const PipelineConfigInfo& configInfo);
		void CreateShaderModule(const std::vector<char>& code,
		                        VkShaderModule* shaderModule);
	};
}
