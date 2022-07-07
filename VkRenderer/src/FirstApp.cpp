#include "FirstApp.hpp"
#include <array>
#include <stdexcept>

namespace Ark
{
	FirstApp::FirstApp()
	{
		LoadModels();
		CreatePipelineLayout();
		CreatePipeline();
		CreateCommandBuffers();
	}

	FirstApp::~FirstApp()
	{
		vkDestroyPipelineLayout(m_arkDevice.Device(), m_pipelineLayout,
		                        nullptr);
	}

	void FirstApp::CreateCommandBuffers()
	{
		m_commandBuffers.resize(m_arkSwapChain.ImageCount());
		VkCommandBufferAllocateInfo allocateInfo{};
		allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocateInfo.commandPool = m_arkDevice.GetCommandPool();
		allocateInfo.commandBufferCount = static_cast<uint32_t>(m_commandBuffers
			.size());
		if (vkAllocateCommandBuffers(m_arkDevice.Device(), &allocateInfo,
		                             m_commandBuffers.data()) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to allocate command buffers!");
		}
		for (int i = 0; i < m_commandBuffers.size(); ++i)
		{
			VkCommandBufferBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			if (vkBeginCommandBuffer(m_commandBuffers[i], &beginInfo) !=
				VK_SUCCESS)
			{
				throw std::runtime_error(
					"failed to begin recording command buffers!");
			}
			VkRenderPassBeginInfo renderPassInfo{};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassInfo.renderPass = m_arkSwapChain.GetRenderPass();
			renderPassInfo.framebuffer = m_arkSwapChain.GetFrameBuffer(i);
			renderPassInfo.renderArea.offset = {0, 0};
			renderPassInfo.renderArea.extent = m_arkSwapChain.
				GetSwapChainExtent();

			std::array<VkClearValue, 2> clearValues{};
			clearValues[0].color = {0.1f, 0.1f, 0.1f, 1.0f};
			clearValues[1].depthStencil = {1.0f, 0};
			renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.
				size());
			renderPassInfo.pClearValues = clearValues.data();

			vkCmdBeginRenderPass(m_commandBuffers[i], &renderPassInfo,
			                     VK_SUBPASS_CONTENTS_INLINE);

			m_arkPipeline->Bind(m_commandBuffers[i]);
			//vkCmdDraw(m_commandBuffers[i], 3, 1, 0, 0);
			m_arkModel->Bind(m_commandBuffers[i]);
			m_arkModel->Draw(m_commandBuffers[i]);
			vkCmdEndRenderPass(m_commandBuffers[i]);
			if (vkEndCommandBuffer(m_commandBuffers[i]) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to record command buffer!");
			}
		}
	}

	void FirstApp::DrawFrame()
	{
		uint32_t imageIndex;
		auto result = m_arkSwapChain.AcquireNextImage(&imageIndex);
		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			throw std::runtime_error("failed to acquire swap chain image!");
		}
		result = m_arkSwapChain.SubmitCommandBuffers(
			&m_commandBuffers[imageIndex], &imageIndex);
		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("failed to represent swap chain image!");
		}
	}


	void FirstApp::Run()
	{
		while (!m_window.ShouldClose())
		{
			glfwPollEvents();
			DrawFrame();
		}
		vkDeviceWaitIdle(m_arkDevice.Device());
	}

	void FirstApp::CreatePipelineLayout()
	{
		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType =
			VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pSetLayouts = nullptr;
		pipelineLayoutInfo.pushConstantRangeCount = 0;
		pipelineLayoutInfo.pPushConstantRanges = nullptr;
		if (vkCreatePipelineLayout(m_arkDevice.Device(), &pipelineLayoutInfo,
		                           nullptr, &m_pipelineLayout) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create pipeline layout!");
		}
	}

	void FirstApp::CreatePipeline()
	{
		PipelineConfigInfo pipelineConfig{};
		ArkPipeline::DefaultPipelineConfigInfo(
			pipelineConfig,
			m_arkSwapChain.Width(), m_arkSwapChain.Height());
		pipelineConfig.renderPass = m_arkSwapChain.GetRenderPass();
		pipelineConfig.pipelineLayout = m_pipelineLayout;
		m_arkPipeline = std::make_unique<ArkPipeline>(
			m_arkDevice, "shaders/triangle.vert.spv",
			"shaders/triangle.frag.spv", pipelineConfig);
	}

	void FirstApp::Sierpinski(std::vector<ArkModel::Vertex>& vertices,
		int depth,
		glm::vec2 left,
		glm::vec2 right,
		glm::vec2 top) {
		if (depth <= 0) {
			vertices.push_back({ top });
			vertices.push_back({ right });
			vertices.push_back({ left });
		}
		else {
			auto leftTop = 0.5f * (left + top);
			auto rightTop = 0.5f * (right + top);
			auto leftRight = 0.5f * (left + right);
			Sierpinski(vertices, depth - 1, left, leftRight, leftTop);
			Sierpinski(vertices, depth - 1, leftRight, right, rightTop);
			Sierpinski(vertices, depth - 1, leftTop, rightTop, top);
		}
	}
	void FirstApp::LoadModels()
	{
		//std::vector<ArkModel::Vertex> vertices{
		//	{{0.0, -0.5}},
		//	{{0.5, 0.5}},
		//	{{-0.5, 0.5}},
		//};
		std::vector<ArkModel::Vertex> vertices{};
		Sierpinski(vertices, 5, { -0.5f, 0.5f }, { 0.5f, 0.5f }, { 0.0f, -0.5f });
		m_arkModel = std::make_unique<ArkModel>(m_arkDevice, vertices);
	}
}
