#include "FirstApp.hpp"
#include <array>
#include <stdexcept>

namespace Ark
{
	FirstApp::FirstApp()
	{
		LoadModels();
		CreatePipelineLayout();
		RecreateSwapChain();
		CreateCommandBuffers();
	}

	FirstApp::~FirstApp()
	{
		vkDestroyPipelineLayout(m_arkDevice.Device(), m_pipelineLayout,
		                        nullptr);
	}

	void FirstApp::CreateCommandBuffers()
	{
		m_commandBuffers.resize(m_arkSwapChain->ImageCount());
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
		//for (int i = 0; i < m_commandBuffers.size(); ++i)
		//{
		//	
		//}
	}

	void FirstApp::RecordCommandBuffer(int imageIndex)
	{
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		if (vkBeginCommandBuffer(m_commandBuffers[imageIndex], &beginInfo) !=
			VK_SUCCESS)
		{
			throw std::runtime_error(
				"failed to begin recording command buffers!");
		}
		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = m_arkSwapChain->GetRenderPass();
		renderPassInfo.framebuffer = m_arkSwapChain->GetFrameBuffer(imageIndex);
		renderPassInfo.renderArea.offset = {0, 0};
		renderPassInfo.renderArea.extent = m_arkSwapChain->
			GetSwapChainExtent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = {0.1f, 0.1f, 0.1f, 1.0f};
		clearValues[1].depthStencil = {1.0f, 0};
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.
			size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(m_commandBuffers[imageIndex], &renderPassInfo,
		                     VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(m_arkSwapChain->GetSwapChainExtent()
			.width);
		viewport.height = static_cast<float>(m_arkSwapChain->
		                                     GetSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor{{0, 0}, m_arkSwapChain->GetSwapChainExtent()};
		vkCmdSetViewport(m_commandBuffers[imageIndex], 0, 1, &viewport);
		vkCmdSetScissor(m_commandBuffers[imageIndex], 0, 1, &scissor);
		m_arkPipeline->Bind(m_commandBuffers[imageIndex]);
		//vkCmdDraw(m_commandBuffers[i], 3, 1, 0, 0);
		m_arkModel->Bind(m_commandBuffers[imageIndex]);
		m_arkModel->Draw(m_commandBuffers[imageIndex]);
		vkCmdEndRenderPass(m_commandBuffers[imageIndex]);
		if (vkEndCommandBuffer(m_commandBuffers[imageIndex]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to record command buffer!");
		}
	}

	void FirstApp::RecreateSwapChain()
	{
		auto extent = m_window.GetExtent();
		while (extent.width == 0 || extent.height == 0)
		{
			extent = m_window.GetExtent();
			glfwWaitEvents();
		}
		// wait until the swap chain is no longer being used
		vkDeviceWaitIdle(m_arkDevice.Device());
		if (m_arkSwapChain == nullptr)
		{
			m_arkSwapChain = std::make_unique<
				ArkSwapChain>(m_arkDevice, extent);
		}
		else
		{
			m_arkSwapChain = std::make_unique<ArkSwapChain>(
				m_arkDevice, extent, std::move(m_arkSwapChain));
			if (m_arkSwapChain->ImageCount() != m_commandBuffers.size())
			{
				FreeCommandBuffers();
				CreateCommandBuffers();
			}
		}
		// TODO: if render pass compatible do nothing else
		CreatePipeline();
	}

	void FirstApp::FreeCommandBuffers()
	{
		vkFreeCommandBuffers(m_arkDevice.Device(), m_arkDevice.GetCommandPool(),
		                     static_cast<uint32_t>(m_commandBuffers.size()),
		                     m_commandBuffers.data());
		m_commandBuffers.clear();
	}


	void FirstApp::DrawFrame()
	{
		uint32_t imageIndex;
		auto result = m_arkSwapChain->AcquireNextImage(&imageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			RecreateSwapChain();
			return;
		}
		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			throw std::runtime_error("failed to acquire swap chain image!");
		}
		RecordCommandBuffer(imageIndex);
		result = m_arkSwapChain->SubmitCommandBuffers(
			&m_commandBuffers[imageIndex], &imageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
			m_window.WasWindowResized())
		{
			m_window.ResetWindowResizedFlag();
			RecreateSwapChain();
			return;
		}
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
		assert(m_arkSwapChain != nullptr,
		       "Cannot create pipeline before swap chain");
		assert(m_pipelineLayout != nullptr,
		       "Cannot create pipeline before pipeline layout");
		PipelineConfigInfo pipelineConfig{};
		ArkPipeline::DefaultPipelineConfigInfo(
			pipelineConfig);
		pipelineConfig.renderPass = m_arkSwapChain->GetRenderPass();
		pipelineConfig.pipelineLayout = m_pipelineLayout;
		m_arkPipeline = std::make_unique<ArkPipeline>(
			m_arkDevice, "shaders/triangle.vert.spv",
			"shaders/triangle.frag.spv", pipelineConfig);
	}

	void FirstApp::Sierpinski(std::vector<ArkModel::Vertex>& vertices,
	                          int depth,
	                          glm::vec2 left,
	                          glm::vec2 right,
	                          glm::vec2 top)
	{
		if (depth <= 0)
		{
			vertices.push_back({top});
			vertices.push_back({right});
			vertices.push_back({left});
		}
		else
		{
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
		std::vector<ArkModel::Vertex> vertices{
			{{0.0, -0.5}, {1.0f, 0.0f, 0.0f}},
			{{0.5, 0.5}, {0.0f, 1.0f, 0.0f}},
			{{-0.5, 0.5}, {0.0f, 0.0f, 1.0f}},
		};
		//std::vector<ArkModel::Vertex> vertices{};
		//Sierpinski(vertices, 5, { -0.5f, 0.5f }, { 0.5f, 0.5f }, { 0.0f, -0.5f });
		m_arkModel = std::make_unique<ArkModel>(m_arkDevice, vertices);
	}
}
