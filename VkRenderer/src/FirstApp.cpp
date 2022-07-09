#include "FirstApp.hpp"
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
		glm::mat2 transform{ 1.0f };
		glm::vec2 offset;
		alignas(16) glm::vec3 color;
	};

	FirstApp::FirstApp()
	{
		LoadGameObjects();
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
		clearValues[0].color = {0.01f, 0.01f, 0.01f, 1.0f};
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

		RenderGameObjects(m_commandBuffers[imageIndex]);
		vkCmdEndRenderPass(m_commandBuffers[imageIndex]);
		if (vkEndCommandBuffer(m_commandBuffers[imageIndex]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to record command buffer!");
		}
	}

	void FirstApp::RenderGameObjects(VkCommandBuffer commandBuffer)
	{
		m_arkPipeline->Bind(commandBuffer);
		for (auto& obj: m_gameObjects)
		{
			obj.m_transform2d.rotation = glm::mod(obj.m_transform2d.rotation + 0.1f, 360.0f);
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

	void FirstApp::LoadGameObjects()
	{
		std::vector<ArkModel::Vertex> vertices{
			{{0.0, -0.5}, {1.0f, 0.0f, 0.0f}},
			{{0.5, 0.5}, {0.0f, 1.0f, 0.0f}},
			{{-0.5, 0.5}, {0.0f, 0.0f, 1.0f}},
		};
		//std::vector<ArkModel::Vertex> vertices{};
		//Sierpinski(vertices, 5, { -0.5f, 0.5f }, { 0.5f, 0.5f }, { 0.0f, -0.5f });
		auto arkModel = std::make_shared<ArkModel>(m_arkDevice, vertices);
		auto triangle = ArkGameObject::Create();
		triangle.m_model = arkModel;
		triangle.m_color = { .1f, .8f, .1f };
		triangle.m_transform2d.translation.x = .2f;
		triangle.m_transform2d.scale = { 2.f, .5f };
		triangle.m_transform2d.rotation = 90.0f;
		m_gameObjects.push_back(std::move(triangle));
	}
}
