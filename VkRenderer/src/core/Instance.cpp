#include "Instance.hpp"
#include "Debug.hpp"
#include <iostream>
#include <unordered_set>

#include "Initializers.hpp"

namespace Ark
{

  /**
   * \brief Instance Class Constructor
   * \param enableValidationLayers flag for enabling validation layer
   */
  Instance::Instance(const bool enableValidationLayers) : m_enableValidationLayers(enableValidationLayers)
  {
    // Enable Validation Layer if required
    if (enableValidationLayers)
    {
      if (!CheckValidationLayerSupport())
      {
        throw std::runtime_error("validation layers requested, but not available!");
      }
      SetupDebugMessenger();
    }
    GetRequiredInstanceExtensions();
  }

  void Instance::Create()
  {
    // Check if instance has already been created
    if (m_instance != VK_NULL_HANDLE) 
      return;

    // Check extension support before creating instances
    CheckExtensionsSupport();

    VkApplicationInfo applicationInfo = {};
    applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    applicationInfo.pApplicationName = "VkRenderer";
    applicationInfo.pEngineName = "ArkEngine";
    applicationInfo.applicationVersion = VK_MAKE_VERSION(0, 1, 0);
    applicationInfo.engineVersion = VK_MAKE_VERSION(0, 1, 0);
    applicationInfo.apiVersion = VK_API_VERSION_1_3;

    VkInstanceCreateInfo instanceCreateInfo = Initializer::InstanceCreateInfo();
    instanceCreateInfo.pApplicationInfo = &applicationInfo;

    instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(m_requiredExtensions.size());
    instanceCreateInfo.ppEnabledExtensionNames = m_requiredExtensions.data();
    if (m_enableValidationLayers)
    {
      instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(validationLayers.size());
      instanceCreateInfo.ppEnabledLayerNames = validationLayers.data();
    }
    else
    {
      instanceCreateInfo.enabledLayerCount = 0;
    }
    VK_CHECK_RESULT(vkCreateInstance(&instanceCreateInfo, nullptr, &m_instance), "Vulkan Instance creation");
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

  void Instance::GetRequiredInstanceExtensions()
  {
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if (m_enableValidationLayers)
    {
      extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }
    for (auto& extension : extensions)
    {
      m_requiredExtensions.push_back(extension);
    }
  }

  void Instance::CheckExtensionsSupport()
  {
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    if (extensionCount > 0)
    {
      std::vector<VkExtensionProperties> extensions(extensionCount);
      vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
      for (VkExtensionProperties extension : extensions)
      {
        m_supportedExtensions.push_back(extension.extensionName);
      }
    }
    std::cout << "available extensions:" << std::endl;
    std::unordered_set<std::string> available;
    for (const auto& extension : m_supportedExtensions)
    {
      std::cout << "\t" << extension << std::endl;
      available.insert(extension);
    }

    std::cout << "required extensions:" << std::endl;
    for (const auto& required : m_requiredExtensions)
    {
      std::cout << "\t" << required << std::endl;
      if (std::find(m_supportedExtensions.begin(), m_supportedExtensions.end(), required) ==
        m_supportedExtensions.end())
      {
        throw std::runtime_error("Missing required glfw extension");
      }
    }
  }

  bool Instance::CheckValidationLayerSupport()
  {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char* layerName : validationLayers)
    {
      bool layerFound = false;

      for (const auto& layerProperties : availableLayers)
      {
        if (strcmp(layerName, layerProperties.layerName) == 0)
        {
          layerFound = true;
          break;
        }
      }

      if (!layerFound)
      {
        return false;
      }
    }

    return true;
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
