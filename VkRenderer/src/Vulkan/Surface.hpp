#pragma once
#include "Vulkan.hpp"

namespace Ark
{
  class WindowSystem;
}

namespace Vulkan
{
  class Instance;
  class Window;

  class Surface final
  {
  public:
    VULKAN_NON_COPIABLE(Surface)
    explicit Surface(const Instance& instance, Ark::WindowSystem& window);
    ~Surface();
    const class Instance& Instance() const { return m_instance; }

  private:
    const class Instance& m_instance;
    VULKAN_HANDLE(VkSurfaceKHR, m_surface);
  };
}
