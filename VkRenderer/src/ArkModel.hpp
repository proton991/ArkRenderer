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
      glm::vec3 position{};
      glm::vec3 color{};
      glm::vec3 normal{};
      glm::vec2 uv{};
      static std::vector<VkVertexInputBindingDescription> GetBindingDescriptions();
      static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();

      bool operator==(const Vertex& other) const
      {
        return position == other.position && color == other.color && normal == other.normal && uv == other.uv;
      }
    };

    struct Builder
    {
      std::vector<Vertex> vertices{};
      std::vector<uint32_t> indices{};

      void LoadModel(const std::string& filePath);
    };

    static std::unique_ptr<ArkModel> CreateModelFromFile(ArkDevice& device, const std::string& filePath);

    ArkModel(ArkDevice& device, const ArkModel::Builder& builder);
    ~ArkModel();
    ArkModel(const ArkModel&) = delete;
    ArkModel& operator=(const ArkModel&) = delete;
    void Bind(VkCommandBuffer commandBuffer);
    void Draw(VkCommandBuffer commandBuffer);
  private:
    void CreateVertexBuffers(const std::vector<Vertex>& vertices);
    void CreateIndexBuffers(const std::vector<uint32_t>& indices);

    bool m_hasIndexBuffer = false;
    ArkDevice& m_arkDevice;

    VkBuffer m_vertexBuffer;
    VkDeviceMemory m_vertexBufferMemory;
    uint32_t m_vertexCount;


    VkBuffer m_indexBuffer;
    VkDeviceMemory m_indexBufferMemory;
    uint32_t m_indexCount;
  };
}

using ModelPtr = std::shared_ptr<Ark::ArkModel>;
