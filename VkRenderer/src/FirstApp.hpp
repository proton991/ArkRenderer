#pragma once
#include "WindowSystem.hpp"
#include "ArkPipleline.hpp"
#include "ArkDevice.hpp"
#include "ArkSwapChain.hpp"
#include "ArkModel.hpp"
#include <memory>
namespace Ark
{
	class FirstApp
	{
	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;
		FirstApp();
		~FirstApp();

		FirstApp(const FirstApp&) = delete;
		FirstApp& operator=(const FirstApp&) = delete;
		void Run();
	private:
		void RecreateSwapChain();
		void RecordCommandBuffer(int imageIndex);
		void Sierpinski(std::vector<ArkModel::Vertex>& vertices,
			int depth,
			glm::vec2 left,
			glm::vec2 right,
			glm::vec2 top);
		void LoadModels();
		void CreatePipelineLayout();
		void CreatePipeline();
		void CreateCommandBuffers();
		void FreeCommandBuffers();
		void DrawFrame();
		WindowSystem m_window{ WIDTH, HEIGHT, "Hello Vulkan!" };
		ArkDevice m_arkDevice{ m_window };
		std::unique_ptr<ArkSwapChain> m_arkSwapChain;
		std::unique_ptr<ArkPipeline> m_arkPipeline;
		VkPipelineLayout m_pipelineLayout;
		std::vector<VkCommandBuffer> m_commandBuffers;
		ModelPtr m_arkModel;
	};

}

