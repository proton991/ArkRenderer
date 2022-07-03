#pragma once

#include "WindowSystem.hpp"

// std lib headers
#include <string>
#include <vector>

namespace Ark
{
	struct SwapChainSupportDetails
	{
		VkSurfaceCapabilitiesKHR m_capabilities;
		std::vector<VkSurfaceFormatKHR> m_formats;
		std::vector<VkPresentModeKHR> m_presentModes;
	};

	struct QueueFamilyIndices
	{
		uint32_t m_graphicsFamily;
		uint32_t m_presentFamily;
		bool m_graphicsFamilyHasValue = false;
		bool m_presentFamilyHasValue = false;

		bool IsComplete()
		{
			return m_graphicsFamilyHasValue && m_presentFamilyHasValue;
		}
	};

	class ArkDevice
	{
	public:
#ifdef NDEBUG
        const bool enableValidationLayers = false;
#else
		const bool enableValidationLayers = true;
#endif

		ArkDevice(WindowSystem& window);
		~ArkDevice();

		// Not copyable or movable
		ArkDevice(const ArkDevice&) = delete;
		ArkDevice& operator=(const ArkDevice&) = delete;
		ArkDevice(ArkDevice&&) = delete;
		ArkDevice& operator=(ArkDevice&&) = delete;

		VkCommandPool GetCommandPool() { return m_commandPool; }
		VkDevice Device() { return m_device; }
		VkSurfaceKHR Surface() { return m_surface; }
		VkQueue GraphicsQueue() { return m_graphicsQueue; }
		VkQueue PresentQueue() { return m_presentQueue; }

		SwapChainSupportDetails GetSwapChainSupport()
		{
			return QuerySwapChainSupport(m_physicalDevice);
		}

		uint32_t FindMemoryType(uint32_t typeFilter,
		                        VkMemoryPropertyFlags properties);

		QueueFamilyIndices FindPhysicalQueueFamilies()
		{
			return FindQueueFamilies(m_physicalDevice);
		}

		VkFormat FindSupportedFormat(
			const std::vector<VkFormat>& candidates, VkImageTiling tiling,
			VkFormatFeatureFlags features);

		// Buffer Helper Functions
		void CreateBuffer(
			VkDeviceSize size,
			VkBufferUsageFlags usage,
			VkMemoryPropertyFlags properties,
			VkBuffer& buffer,
			VkDeviceMemory& bufferMemory);
		VkCommandBuffer BeginSingleTimeCommands();
		void EndSingleTimeCommands(VkCommandBuffer commandBuffer);
		void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer,
		                VkDeviceSize size);
		void CopyBufferToImage(
			VkBuffer buffer, VkImage image, uint32_t width, uint32_t height,
			uint32_t layerCount);

		void CreateImageWithInfo(
			const VkImageCreateInfo& imageInfo,
			VkMemoryPropertyFlags properties,
			VkImage& image,
			VkDeviceMemory& imageMemory);

		VkPhysicalDeviceProperties properties;

	private:
		void CreateInstance();
		void SetupDebugMessenger();
		void CreateSurface();
		void PickPhysicalDevice();
		void CreateLogicalDevice();
		void CreateCommandPool();

		// helper functions
		bool IsDeviceSuitable(VkPhysicalDevice device);
		std::vector<const char*> GetRequiredExtensions();
		bool CheckValidationLayerSupport();
		QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
		void PopulateDebugMessengerCreateInfo(
			VkDebugUtilsMessengerCreateInfoEXT& createInfo);
		void HasGlfwRequiredInstanceExtensions();
		bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
		SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);

		VkInstance m_instance;
		VkDebugUtilsMessengerEXT m_debugMessenger;
		VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
		WindowSystem& m_window;
		VkCommandPool m_commandPool;

		VkDevice m_device;
		VkSurfaceKHR m_surface;
		VkQueue m_graphicsQueue;
		VkQueue m_presentQueue;

		const std::vector<const char*> validationLayers = {
			"VK_LAYER_KHRONOS_validation"
		};
		const std::vector<const char*> deviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};
	};
} // namespace lve
