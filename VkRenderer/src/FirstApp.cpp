#include "FirstApp.hpp"
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
	FirstApp::FirstApp()
	{
		LoadGameObjects();
	}

	FirstApp::~FirstApp()
	{
	}


	void FirstApp::Run()
	{
		SimpleRenderSystem simpleRenderSystem{
			m_arkDevice, m_arkRenderer.GetSwapChainRenderPass()
		};
		while (!m_window.ShouldClose())
		{
			glfwPollEvents();
			if (auto commandBuffer = m_arkRenderer.BeginFrame())
			{
				m_arkRenderer.BeginSwapChainRenderPass(commandBuffer);
				simpleRenderSystem.RenderGameObjects(
					commandBuffer, m_gameObjects);
				m_arkRenderer.EndSwapChainRenderPass(commandBuffer);
				m_arkRenderer.EndFrame();
			}
		}
		vkDeviceWaitIdle(m_arkDevice.Device());
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
		triangle.m_color = {.1f, .8f, .1f};
		triangle.m_transform2d.translation.x = .2f;
		triangle.m_transform2d.scale = {2.f, .5f};
		triangle.m_transform2d.rotation = 90.0f;
		m_gameObjects.push_back(std::move(triangle));
	}
}
