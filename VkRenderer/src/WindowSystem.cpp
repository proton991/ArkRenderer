#include "WindowSystem.hpp"
#include "InputController.hpp"
#include <stdexcept>

namespace Ark
{
  WindowSystem::WindowSystem(const int w, const int h, const std::string& name) : m_width(w), m_height(h),
    m_windowName(name)
  {
    Init();
  }

  void WindowSystem::Init()
  {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    m_window = glfwCreateWindow(m_width, m_height, m_windowName.c_str(), nullptr, nullptr);
    glfwSetWindowUserPointer(m_window, this);
    glfwSetFramebufferSizeCallback(m_window, FrameBufferResizedCallback);
    InputManager::ConnectWindowInstanceToInput(m_window);
    DisableCursor();
  }

  WindowSystem::~WindowSystem()
  {
    glfwDestroyWindow(m_window);
    glfwTerminate();
  }

  void WindowSystem::CreateWindowSurface(VkInstance instance, VkSurfaceKHR* surface)
  {
    if (glfwCreateWindowSurface(instance, m_window, nullptr, surface) != VK_SUCCESS)
    {
      throw std::runtime_error("failed to create window surface!");
    }
  }

  void WindowSystem::EnableCursor() const
  {
    glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  }

  /***********************************************************************************/
  void WindowSystem::DisableCursor() const
  {
    glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  }

  void WindowSystem::Update()
  {
    glfwPollEvents();

    if (InputManager::GetInstance().IsKeyPressed(GLFW_KEY_TAB))
    {
      m_showCursor = !m_showCursor;
      if (m_showCursor)
      {
        EnableCursor();
      }
      else
      {
        DisableCursor();
      }
    }

    // Check if the window needs to be closed
    if (InputManager::GetInstance().IsKeyPressed(GLFW_KEY_ESCAPE) || glfwWindowShouldClose(m_window))
    {
      m_shouldClose = true;
      glfwSetWindowShouldClose(m_window, true);
    }
  }

  void WindowSystem::FrameBufferResizedCallback(GLFWwindow* window, int width, int height)
  {
    auto arkWindow = reinterpret_cast<WindowSystem*>(glfwGetWindowUserPointer(window));
    arkWindow->m_frameBufferResized = true;
    arkWindow->m_width = width;
    arkWindow->m_height = height;
  }
}
