#pragma once
#include "ArkCamera.hpp"
#include "ArkDescriptors.hpp"
#include "ArkDevice.hpp"
#include "ArkGameObject.hpp"
#include "ArkRenderer.hpp"
#include "WindowSystem.hpp"

namespace Ark
{
  class AppBase
  {
  public:
    static constexpr int WIDTH = 800;
    static constexpr int HEIGHT = 600;
    virtual void Run() = 0;

    AppBase();
  private:
    virtual void LoadAssets() = 0;
    std::string m_windowTitle = "Hello Vulkan!";
    WindowSystem m_window;
    ArkDevice m_arkDevice;
    ArkRenderer m_arkRenderer;
    std::unique_ptr<ArkDescriptorPool> m_globalPool{};
    ArkGameObject::Map m_gameObjects;

    ArkCamera m_camera{
      glm::vec3(.0f, .0f, -2.5f), glm::vec3(0.f, 0.f, 0.f), glm::radians(70.0f),
      m_arkRenderer.GetAspectRatio(), 0.1f, 100.0f
    };
  };
}
