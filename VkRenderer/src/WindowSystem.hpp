#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>

namespace Ark
{
	class WindowSystem
	{
	private:
		void Init();
		static void FrameBufferResizedCallback(GLFWwindow* window, int width,
		                                       int height);
		int m_width;
		int m_height;
		bool m_frameBufferResized = false;
		std::string m_windowName;
		GLFWwindow* m_window;

	public:
		WindowSystem(int w, int h, const std::string& name);
		~WindowSystem();
		WindowSystem(WindowSystem&&) = default;
		// Disable copying
		WindowSystem(const WindowSystem&) = delete;
		WindowSystem& operator=(const WindowSystem&) = delete;
		void CreateWindowSurface(VkInstance instance, VkSurfaceKHR* surface);
		bool ShouldClose() const { return glfwWindowShouldClose(m_window); }

		VkExtent2D GetExtent() const
		{
			return {
				static_cast<uint32_t>(m_width), static_cast<uint32_t>(m_height)
			};
		}

		bool WasWindowResized() const { return m_frameBufferResized; }
		void ResetWindowResizedFlag() { m_frameBufferResized = false; }
	};
}
