#include "Instance.hpp"
#include "Enumerate.hpp"
#include <stdexcept>

#include "WindowSystem.hpp"

namespace
{
}

namespace Vulkan
{
  Instance::Instance(const std::vector<const char*>& validationLayers,
                     uint32_t vulkanVersion)
    : m_validationLayers(validationLayers)
  {
    CheckVulkanMinVersion(vulkanVersion);

    auto extensions = GetRequiredExtensions();

    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "VK Renderer";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = vulkanVersion;

    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();
    createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
    createInfo.ppEnabledLayerNames = validationLayers.data();

    Check(vkCreateInstance(&createInfo, nullptr, &m_instance),
          "create instance");

    GetVulkanPhysicalDevices();
    GetVulkanLayers();
    GetSupportedExtensions();
  }

  Instance::~Instance()
  {
    if (m_instance != nullptr)
    {
      vkDestroyInstance(m_instance, nullptr);
      m_instance = nullptr;
    }
  }

  std::vector<const char*> Instance::GetRequiredExtensions() const
  {
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    auto extensions = std::vector<const char*>{glfwExtensions, glfwExtensions + glfwExtensionCount};
    CheckVulkanValidationLayerSupport(m_validationLayers);
    if (!m_validationLayers.empty())
    {
      extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }
    return extensions;
  }

  void Instance::GetSupportedExtensions()
  {
    GetEnumerateVector(static_cast<const char*>(nullptr), vkEnumerateInstanceExtensionProperties, m_extensions);
  }

  void Instance::GetVulkanLayers()
  {
    GetEnumerateVector(vkEnumerateInstanceLayerProperties, m_layers);
  }

  void Instance::GetVulkanPhysicalDevices()
  {
    GetEnumerateVector(m_instance, vkEnumeratePhysicalDevices, m_physicalDevices);
    if (m_physicalDevices.empty())
    {
      throw std::runtime_error("No Vulkan physical devices found!");
    }
  }

  void Instance::CheckVulkanMinVersion(uint32_t minVersion)
  {
    uint32_t version;
    Check(vkEnumerateInstanceVersion(&version), "Enumerate instance version");
    if (minVersion > version)
    {
      throw std::runtime_error("Vulkan version does not meet requirements!");
    }
  }

  void Instance::CheckVulkanValidationLayerSupport(const std::vector<const char*>& validationLayers)
  {
    const auto availableLayers = GetEnumerateVector(vkEnumerateInstanceLayerProperties);

    for (const char* layer : validationLayers)
    {
      auto result = std::find_if(availableLayers.begin(), availableLayers.end(), [layer](const VkLayerProperties& layerProperties)
      {
        return strcmp(layer, layerProperties.layerName) == 0;
      });

      if (result == availableLayers.end())
      {
        throw std::runtime_error("could not find the requested validation layer: '" + std::string(layer) + "'");
      }
    }
  }
}
