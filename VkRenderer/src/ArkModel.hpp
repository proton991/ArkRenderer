#pragma once
#include "ArkDevice.hpp"

//libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

// std
#include <memory>
#include <vector>
namespace Ark
{
	class ArkModel
	{
	public:
		struct Vertex
		{
			glm::vec2 position;
			static std::vector<VkVertexInputBindingDescription> GetBindingDescriptions();
			static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();
		};
		ArkModel(ArkDevice& device, const std::vector<Vertex>& vertices);
		~ArkModel();
		ArkModel(const ArkModel&) = delete;
		ArkModel& operator=(const ArkModel&) = delete;
		void Bind(VkCommandBuffer commandBuffer);
		void Draw(VkCommandBuffer commandBuffer);
	private:
		void CreateVertexBuffers(const std::vector<Vertex>& vertices);

		ArkDevice& m_arkDevice;
		VkBuffer m_vertexBuffer;
		VkDeviceMemory m_vertexBufferMemory;
		uint32_t m_vertexCount;
	};
}
using ModelPtr = std::shared_ptr<Ark::ArkModel>;