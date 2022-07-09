#pragma once
#include "WindowSystem.hpp"
#include "ArkDevice.hpp"
#include "ArkSwapChain.hpp"
#include <memory>
#include <cassert>

namespace Ark
{
	class ArkRenderer
	{
	public:
		ArkRenderer(WindowSystem& window, ArkDevice& device);
		~ArkRenderer();

		ArkRenderer(const ArkRenderer&) = delete;
		ArkRenderer& operator=(const ArkRenderer&) = delete;

		bool IsFrameInProgress() const { return m_isFrameStarted; }

		VkCommandBuffer GetCurrentCommandBuffer() const
		{
			assert(
				m_isFrameStarted &&
				"Cannot get command buffer when frame is not in progress");
			return m_commandBuffers[m_frameIndex];
		}

		VkRenderPass GetSwapChainRenderPass() const
		{
			return m_arkSwapChain->GetRenderPass();
		}

		int GetFrameIndex() const
		{
			assert(
				m_isFrameStarted &&
				"Cannot get frame index when frame not in progress");
			return m_frameIndex;
		}

		VkCommandBuffer BeginFrame();
		void EndFrame();

		void BeginSwapChainRenderPass(VkCommandBuffer commandBuffer);
		void EndSwapChainRenderPass(VkCommandBuffer commandBuffer);
	private:
		void CreateCommandBuffers();
		void RecreateSwapChain();
		void FreeCommandBuffers();

		WindowSystem& m_window;
		ArkDevice& m_arkDevice;
		std::unique_ptr<ArkSwapChain> m_arkSwapChain;
		std::vector<VkCommandBuffer> m_commandBuffers;

		uint32_t m_imageIndex;
		int m_frameIndex{0};
		bool m_isFrameStarted{false};
	};
}
