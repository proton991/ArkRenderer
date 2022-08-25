#include "ArkAppBase.hpp"

namespace Ark
{
  AppBase::AppBase() : m_windowTitle{ "m_windowTitle" }, m_window{ WIDTH, HEIGHT, m_windowTitle },
    m_arkDevice{ m_window }, m_arkRenderer{ m_window, m_arkDevice }
  {
  }
}
