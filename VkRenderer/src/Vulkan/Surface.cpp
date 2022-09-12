#include "Surface.hpp"

#include "Instance.hpp"
#include "WindowSystem.hpp"

namespace Vulkan
{
  Surface::Surface(const Vulkan::Instance& instance, Ark::WindowSystem& window)
    : m_instance(instance)
  {
    Check(glfwCreateWindowSurface(m_instance.Handle(), window.Handle(), nullptr, &m_surface)
          ,"Create window surface");
  }

  Surface::~Surface()
  {
  }
}
