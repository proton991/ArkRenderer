#pragma once

//std
#include <array>
#include <cassert>
#include <functional>

//libs
#include <GLFW/glfw3.h>

namespace Ark
{
  class InputManager
  {
  public:
    InputManager()
    {
      std::fill(m_keys.begin(), m_keys.end(), false);
      std::fill(m_prevKeys.begin(), m_prevKeys.end(), false);
    }

    ~InputManager() = default;

    InputManager(const InputManager&) = delete;
    InputManager& operator=(const InputManager&) = delete;

    static auto& GetInstance()
    {
      static InputManager instance;
      return instance;
    }

    static void ConnectWindowInstanceToInput(GLFWwindow* window)
    {
      const auto keyCallback = [](GLFWwindow* w, auto key, auto scancode,
                                  auto action, auto mode)
      {
        InputManager::GetInstance().keyPressed(
          key, scancode, action, mode);
      };
      glfwSetKeyCallback(window, keyCallback);
      const auto cursorPosCallback = [
        ](GLFWwindow* w, auto xPos, auto yPos)
      {
        InputManager::GetInstance().mouseMoved(xPos, yPos);
      };
      glfwSetCursorPosCallback(window, cursorPosCallback);
    }

    void Update()
    {
      m_mouseMoved = false;
      std::copy(m_keys.cbegin(), m_keys.cend(), m_prevKeys.begin());
    }

    // Was the key only tapped?
    auto IsKeyPressed(const std::size_t key) const noexcept
    {
#ifdef _DEBUG
      assert(key < 1024);
#endif
      return m_keys[key] && !m_prevKeys[key];
    }


    // Is the key being held down?
    auto IsKeyHeld(const std::size_t key) const noexcept
    {
#ifdef _DEBUG
      assert(key < 1024);
#endif
      return m_keys[key];
    }

    // Mouse
    auto MouseMoved() const noexcept { return m_mouseMoved; }
    auto GetMouseX() const noexcept { return m_xPos; }
    auto GetMouseY() const noexcept { return m_yPos; }

    // Generic Input Callbacks
    // Mouse moved
    std::function<void(double, double)> mouseMoved = [&
      ](auto xPos, auto yPos)
    {
      this->m_mouseMoved = true;
      this->m_xPos = xPos;
      this->m_yPos = yPos;
    };

    // Key Pressed
    std::function<void(int, int, int, int)> keyPressed = [&
      ](auto key, auto scancode, auto action, auto mode)
    {
      if (key >= 0 && key < 1024)
      {
        switch (action)
        {
        // Pressed
        case 1:
          this->m_keys[key] = true;
          break;
        case 0:
          this->m_keys[key] = false;
          break;
        }
      }
    };
  private:
    // Keyboard
    std::array<bool, 1024> m_keys;
    std::array<bool, 1024> m_prevKeys;

    // Mouse
    bool m_mouseMoved = false;
    double m_xPos, m_yPos;
  };
}
