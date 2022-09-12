#include "DebugUtils.hpp"
#include <stdexcept>

namespace Vulkan
{
  DebugUtils::DebugUtils(VkInstance instance)
    : fpSetDebugUtilsObjectNameEXT(
      reinterpret_cast<PFN_vkSetDebugUtilsObjectNameEXT>(vkGetInstanceProcAddr(
        instance, "vkSetDebugUtilsObjectNameEXT")))
  {
    if (fpSetDebugUtilsObjectNameEXT == nullptr)
    {
      throw std::runtime_error("failed to get address of 'vkSetDebugUtilsObjectNameEXT'");
    }
  }


  template <typename T>
  void DebugUtils::SetObjectName(const T& obj, const char* name, VkObjectType type) const
  {
    VkDebugUtilsObjectNameInfoEXT info{};
    info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
    info.pNext = nullptr;
    info.objectHandle = reinterpret_cast<const uint64_t&>(obj);
    info.objectType = type;
    info.pObjectName = name;
    Check(fpSetDebugUtilsObjectNameEXT(m_device, &info), "Set object name");
  }
}
