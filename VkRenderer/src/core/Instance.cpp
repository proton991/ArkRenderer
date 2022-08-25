#include "Instance.hpp"
#include "Debug.hpp"
#include <iostream>
#include <unordered_set>

#include "Initializers.hpp"

namespace Ark
{

  Instance::Instance(const bool enableValidationLayers)
    : m_enableValidationLayers(enableValidationLayers)
  {
    VkApplicationInfo applicationInfo = {};
    applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    applicationInfo.pApplicationName = "VkRenderer";
    applicationInfo.pEngineName = "ArkEngine";
    applicationInfo.applicationVersion = VK_MAKE_VERSION(0, 1, 0);
    applicationInfo.engineVersion = VK_MAKE_VERSION(0, 1, 0);
    applicationInfo.apiVersion = VK_API_VERSION_1_3;

    VkInstanceCreateInfo instanceCreateInfo = Initializer::InstanceCreateInfo();
    instanceCreateInfo.pApplicationInfo = &applicationInfo;

    auto extensions = GetRequiredInstanceExtensions();
    instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    instanceCreateInfo.ppEnabledExtensionNames = extensions.data();
    if (enableValidationLayers)
    {
      instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(validationLayers.size());
      instanceCreateInfo.ppEnabledLayerNames = validationLayers.data();
    }
    else
    {
      instanceCreateInfo.enabledLayerCount = 0;
    }
    VK_CHECK_RESULT(vkCreateInstance(&instanceCreateInfo, nullptr, &m_instance),
                    "Vulkan Instance creation");
    HasGlfwRequiredInstanceExtensions();
    if (enableValidationLayers)
    {
      SetupDebugMessenger();
    }
  }

  Instance::~Instance()
  {
    if (m_instance != nullptr)
    {
      vkDestroyInstance(m_instance, nullptr);
      m_instance = nullptr;
    }
    if (m_enableValidationLayers)
    {
      Debug::DestroyDebugUtilsMessengerEXT(m_instance, m_debugMessenger, nullptr);
    }
  }

  std::vector<const char*> Instance::GetRequiredInstanceExtensions() const
  {
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if (m_enableValidationLayers)
    {
      extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }
    return extensions;
  }

  void Instance::HasGlfwRequiredInstanceExtensions() const
  {
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

    std::cout << "available extensions:" << std::endl;
    std::unordered_set<std::string> available;
    for (const auto& extension : extensions)
    {
      std::cout << "\t" << extension.extensionName << std::endl;
      available.insert(extension.extensionName);
    }

    std::cout << "required extensions:" << std::endl;
    auto requiredExtensions = GetRequiredInstanceExtensions();
    for (const auto& required : requiredExtensions)
    {
      std::cout << "\t" << required << std::endl;
      if (available.find(required) == available.end())
      {
        throw std::runtime_error("Missing required glfw extension");
      }
    }
  }


  void Instance::SetupDebugMessenger()
  {
    if (!m_enableValidationLayers) return;
    VkDebugUtilsMessengerCreateInfoEXT createInfo = Initializer::DebugUtilsMessengerCreateInfoEXT();
    if (Debug::CreateDebugUtilsMessengerEXT(m_instance, &createInfo, nullptr, &m_debugMessenger) !=
      VK_SUCCESS)
    {
      throw std::runtime_error("failed to set up debug messenger!");
    }
  }
}
