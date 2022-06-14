#include "ArkEngine.h"
#include <iostream>
#include "../Input.h"
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
}

void ArkEngine::Execute()
{
	while (!m_window.ShouldClose())
	{
		Input::GetInstance().Update();
		m_window.Update();
		m_window.SwapBuffers();
	}
	Shutdown();
}
