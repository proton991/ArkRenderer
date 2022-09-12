#include "Device.hpp"

#include <set>
#include <stdexcept>
#include <string>

#include "Enumerate.hpp"
#include "Instance.hpp"
#include "Surface.hpp"

namespace Vulkan
{
  namespace
  {
    std::vector<VkQueueFamilyProperties>::const_iterator FindQueue(
      const std::vector<VkQueueFamilyProperties>& queueFamilies,
      const std::string& name,
      const VkQueueFlags requiredBits,
      const VkQueueFlags excludedBits)
    {
      const auto family = std::find_if(queueFamilies.begin(), queueFamilies.end(), [requiredBits, excludedBits](const VkQueueFamilyProperties& queueFamily)
      {
        return queueFamily.queueCount > 0 && queueFamily.queueFlags & requiredBits && !(queueFamily.queueFlags & excludedBits);
      });

      if (family == queueFamilies.end())
      {
        throw std::runtime_error("found no matching " + name + " queue");
      }

      return family;
    }
  }

  Device::Device(
    VkPhysicalDevice physicalDevice,
    const Vulkan::Surface& surface,
    const std::vector<const char*>& requiredExtensions,
    const VkPhysicalDeviceFeatures& deviceFeatures,
    const void* nextDeviceFeatures):
    m_physicalDevice(physicalDevice),
    m_surface(surface),
    m_debugUtils(nullptr)
  {
    CheckRequiredExtensions(physicalDevice, requiredExtensions);
    const auto queueFamilies = GetEnumerateVector(physicalDevice, vkGetPhysicalDeviceQueueFamilyProperties);
    // Find the graphics queue.
    const auto graphicsFamily = FindQueue(queueFamilies, "graphics", VK_QUEUE_GRAPHICS_BIT, 0);
    const auto computeFamily = FindQueue(queueFamilies, "compute", VK_QUEUE_COMPUTE_BIT, VK_QUEUE_GRAPHICS_BIT);
    const auto transferFamily = FindQueue(queueFamilies, "transfer", VK_QUEUE_TRANSFER_BIT,
                                          VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT);
    // Find the presentation queue (usually the same as graphics queue).
    const auto presentFamily = std::find_if(queueFamilies.begin(), queueFamilies.end(), [&](const VkQueueFamilyProperties& queueFamily)
    {
      VkBool32 presentSupport = VK_FALSE;
      const uint32_t i = static_cast<uint32_t>(&queueFamily - &*queueFamilies.
        cbegin());
      vkGetPhysicalDeviceSurfaceSupportKHR(
        m_physicalDevice, i, surface.Handle(), &presentSupport);
      return queueFamily.queueCount > 0 && presentSupport;
    });
    if (presentFamily == queueFamilies.end())
    {
      throw std::runtime_error("no presentation queue found");
    }
    m_graphicsQueueFamilyIndex = static_cast<uint32_t>(graphicsFamily - queueFamilies.begin());
    m_computeQueueFamilyIndex = static_cast<uint32_t>(computeFamily - queueFamilies.begin());
    m_presentQueueFamilyIndex = static_cast<uint32_t>(presentFamily - queueFamilies.begin());
    m_transferQueueFamilyIndex = static_cast<uint32_t>(transferFamily - queueFamilies.begin());

    // Queues can be the same
    const std::set<uint32_t> uniqueQueueFamilies =
    {
      m_graphicsQueueFamilyIndex,
      m_computeQueueFamilyIndex,
      m_presentQueueFamilyIndex,
      m_transferQueueFamilyIndex
    };

    // Create queues
    float queuePriority = 1.0f;
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

    for (uint32_t queueFamilyIndex : uniqueQueueFamilies)
    {
      VkDeviceQueueCreateInfo queueCreateInfo = {};
      queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
      queueCreateInfo.queueFamilyIndex = queueFamilyIndex;
      queueCreateInfo.queueCount = 1;
      queueCreateInfo.pQueuePriorities = &queuePriority;
      queueCreateInfos.push_back(queueCreateInfo);
    }

    // Create device
    VkDeviceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pNext = nextDeviceFeatures;
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());
    createInfo.ppEnabledExtensionNames = requiredExtensions.data();

    Check(vkCreateDevice(physicalDevice, &createInfo, nullptr, &m_device),
          "create logical device");

    m_debugUtils.SetDevice(m_device);

    vkGetDeviceQueue(m_device, m_graphicsQueueFamilyIndex, 0, &m_graphicsQueue);
    vkGetDeviceQueue(m_device, m_computeQueueFamilyIndex, 0, &m_computeQueue);
    vkGetDeviceQueue(m_device, m_presentQueueFamilyIndex, 0, &m_presentQueue);
    vkGetDeviceQueue(m_device, m_transferQueueFamilyIndex, 0, &m_transferQueue);
  }

  void Device::WaitIdle() const
  {
    Check(vkDeviceWaitIdle(m_device), "wait for device idle");
  }

  void Device::CheckRequiredExtensions(VkPhysicalDevice physicalDevice,
                                       const std::vector<const char*>& requiredExtensions) const
  {
    const auto availableExtensions = GetEnumerateVector(physicalDevice, static_cast<const char*>(nullptr), vkEnumerateDeviceExtensionProperties);
    std::set<std::string> required(requiredExtensions.begin(), requiredExtensions.end());

    for (const auto& extension : availableExtensions)
    {
      required.erase(extension.extensionName);
    }

    if (!required.empty())
    {
      bool first = true;
      std::string extensions;

      for (const auto& extension : required)
      {
        if (!first)
        {
          extensions += ", ";
        }

        extensions += extension;
        first = false;
      }

      throw std::runtime_error("missing required extensions: " + extensions);
    }
  }
}
