#include "WindowSystem.h"
#include <iostream>
#include "../Input.h"

GLFWwindow* WindowSystem::Init()
{
	const int width = 1024;
	const int height = 768;
	if (!glfwInit())
	{
		std::cerr << "Failed to start GLFW\n";
		glfwTerminate();
		return nullptr;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4); // Enable 4xMSAA
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	m_window = glfwCreateWindow(1024, 768, "ArkRenderer", nullptr, nullptr);
	if (!m_window)
	{
		std::cerr << "Failed to create GLFW m_window.\n";
		glfwTerminate();
		return nullptr;
	}
	glfwMakeContextCurrent(m_window);
	glfwFocusWindow(m_window);
	glfwSwapInterval(1);	//Enable Vsync

		// Center window
	const auto mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	glfwSetWindowPos(m_window, (mode->width / 2) - width / 2, (mode->height / 2) - height / 2);
	glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	return m_window;
}

void WindowSystem::Shutdown() const
{
	std::cout << "Closing Window...\n";
	glfwDestroyWindow(m_window);
	glfwTerminate();
}


void WindowSystem::SwapBuffers() const
{
	glfwSwapBuffers(m_window);
}

void WindowSystem::Update()
{
	glfwPollEvents();
	if (Input::GetInstance().IsKeyPressed(GLFW_KEY_TAB))
	{
		m_showCursor = !m_showCursor;
		if (m_showCursor)
		{
			glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
		else
		{
			glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
	}

	// Check if the m_window needs to be closed
	if (Input::GetInstance().IsKeyPressed(GLFW_KEY_ESCAPE) ||
		glfwWindowShouldClose(m_window))
	{
		m_shouldClose = true;
		glfwSetWindowShouldClose(m_window, true);
	}
}
