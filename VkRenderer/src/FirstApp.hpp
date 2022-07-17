#pragma once
#include "WindowSystem.hpp"
#include "ArkGameObject.hpp"
#include "ArkDevice.hpp"
#include "ArkRenderer.hpp"
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
		void LoadGameObjects();
		WindowSystem m_window{WIDTH, HEIGHT, "Hello Vulkan!"};
		ArkDevice m_arkDevice{m_window};
		ArkRenderer m_arkRenderer{m_window, m_arkDevice};

		std::vector<ArkGameObject> m_gameObjects;
	};
}
