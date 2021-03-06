#include "ArkEngine.h"
#include <iostream>
#include "../Input.h"
#include <GLFW/glfw3.h>

void ConnectToInput(GLFWwindow* window)
{
	const auto resizeCallback = [](GLFWwindow* w, auto width, auto height) {
		Input::GetInstance().windowResized(width, height);
	};
	glfwSetWindowSizeCallback(window, resizeCallback);

	const auto keyCallback = [](GLFWwindow* w, auto key, auto scanCode, auto action, auto mode) {
		Input::GetInstance().keyPressed(key, scanCode, action, mode);
	};
	glfwSetKeyCallback(window, keyCallback);

	const auto cursorPosCallback = [](GLFWwindow* w, auto xPos, auto yPos) {
		Input::GetInstance().mouseMoved(xPos, yPos);
	};
	glfwSetCursorPosCallback(window, cursorPosCallback);
}
void ArkEngine::Shutdown() const
{
	m_window.Shutdown();
}

ArkEngine::ArkEngine()
{
	std::cout << "**************************************************\n";
	std::cout << "Engine starting up...\n";
	auto* window{ m_window.Init() };

	std::cout << "**************************************************\n";
	std::cout << "Initializing Window...\n";
	ConnectToInput(window);
	m_renderer.Init();
}

void ArkEngine::Execute()
{
	float deltaTime = 0.0f;
	float lastFrame = 0.0f;
	while (!m_window.ShouldClose())
	{
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		Input::GetInstance().Update();
		m_window.Update();
		m_camera.Update(deltaTime);
		m_renderer.Render(m_camera);
		m_window.SwapBuffers();
	}
	Shutdown();
}
