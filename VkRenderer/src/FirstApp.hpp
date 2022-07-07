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
		void Sierpinski(std::vector<ArkModel::Vertex>& vertices,
			int depth,
			glm::vec2 left,
			glm::vec2 right,
			glm::vec2 top);
		void LoadModels();
		void CreatePipelineLayout();
		void CreatePipeline();
		void CreateCommandBuffers();
		void DrawFrame();
		WindowSystem m_window{ WIDTH, HEIGHT, "Hello Vulkan!" };
		ArkDevice m_arkDevice{ m_window };
		ArkSwapChain m_arkSwapChain{ m_arkDevice, m_window.GetExtent() };
		//ArkPipeline arkPipeline{ m_arkDevice, "shaders/triangle.vert.spv", "shaders/triangle.frag.spv", ArkPipeline::DefaultPipelineConfigInfo(WIDTH, HEIGHT) };
		std::unique_ptr<ArkPipeline> m_arkPipeline;
		VkPipelineLayout m_pipelineLayout;
		std::vector<VkCommandBuffer> m_commandBuffers;
		ModelPtr m_arkModel;
	};

}

