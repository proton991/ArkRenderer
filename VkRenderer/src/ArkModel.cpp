#include "ArkModel.hpp"
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
#include <cassert>
#include <iostream>
#include <unordered_map>

#include "ArkUtils.hpp"

namespace std
{
  template <>
  struct hash<Ark::ArkModel::Vertex>
  {
    size_t operator()(Ark::ArkModel::Vertex const& vertex) const
    {
      size_t seed = 0;
      Ark::HashCombine(seed, vertex.position, vertex.color, vertex.normal, vertex.uv);
      return seed;
    }
  };
}

namespace Ark
{
  ArkModel::ArkModel(ArkDevice& device, const ArkModel::Builder& builder) : m_arkDevice(device)
  {
    CreateVertexBuffers(builder.vertices);
    CreateIndexBuffers(builder.indices);
  }

  ArkModel::~ArkModel() = default;

  std::unique_ptr<ArkModel> ArkModel::CreateModelFromFile(ArkDevice& device, const std::string& filePath)
  {
    Builder builder{};
    builder.LoadModel(filePath);
    std::cout << "Vertex count: " << builder.vertices.size() << "\n";
    return std::make_unique<ArkModel>(device, builder);
  }

  void ArkModel::CreateVertexBuffers(const std::vector<Vertex>& vertices)
  {
    m_vertexCount = static_cast<uint32_t>(vertices.size());
    assert(m_vertexCount >= 3 && "Vertex count must be at least 3");
    const VkDeviceSize bufferSize = sizeof(vertices[0]) * m_vertexCount;
    uint32_t vertexSize = sizeof(vertices[0]);
    ArkBuffer stagingBuffer{
      m_arkDevice,
      vertexSize,
      m_vertexCount,
      VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    };
    stagingBuffer.Map();
    stagingBuffer.WriteToBuffer((void*)vertices.data());

    m_vertexBuffer = std::make_unique<ArkBuffer>(
      m_arkDevice,
      vertexSize,
      m_vertexCount,
      VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
    );
    m_arkDevice.CopyBuffer(stagingBuffer.GetBuffer(), m_vertexBuffer->GetBuffer(), bufferSize);
  }

  void ArkModel::CreateIndexBuffers(const std::vector<uint32_t>& indices)
  {
    m_indexCount = static_cast<uint32_t>(indices.size());
    m_hasIndexBuffer = m_indexCount > 0;
    if (!m_hasIndexBuffer) return;
    VkDeviceSize bufferSize = sizeof(indices[0]) * m_indexCount;
    uint32_t indexSize = sizeof(indices[0]);
    ArkBuffer stagingBuffer{
      m_arkDevice,
      indexSize,
      m_indexCount,
      VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    };
    stagingBuffer.Map();
    stagingBuffer.WriteToBuffer((void*)indices.data());
    m_indexBuffer = std::make_unique<ArkBuffer>(
      m_arkDevice,
      indexSize,
      m_indexCount,
      VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
    );
    m_arkDevice.CopyBuffer(stagingBuffer.GetBuffer(), m_indexBuffer->GetBuffer(), bufferSize);
  }

  void ArkModel::Bind(VkCommandBuffer commandBuffer)
  {
    VkBuffer buffers[] = {m_vertexBuffer->GetBuffer()};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
    if (m_hasIndexBuffer)
    {
      vkCmdBindIndexBuffer(commandBuffer, m_indexBuffer->GetBuffer(), 0, VK_INDEX_TYPE_UINT32);
    }
  }

  void ArkModel::Draw(VkCommandBuffer commandBuffer)
  {
    if (m_hasIndexBuffer)
    {
      vkCmdDrawIndexed(commandBuffer, m_indexCount, 1, 0, 0, 0);
    }
    else
    {
      vkCmdDraw(commandBuffer, m_vertexCount, 1, 0, 0);
    }
  }

  std::vector<VkVertexInputAttributeDescription> ArkModel::Vertex::GetAttributeDescriptions()
  {
    std::vector<VkVertexInputAttributeDescription> attributeDescriptions(4);
    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[0].offset = offsetof(Vertex, position);

    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[1].offset = offsetof(Vertex, color);

    attributeDescriptions[2].binding = 0;
    attributeDescriptions[2].location = 2;
    attributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[2].offset = offsetof(Vertex, normal);

    attributeDescriptions[3].binding = 0;
    attributeDescriptions[3].location = 3;
    attributeDescriptions[3].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[3].offset = offsetof(Vertex, uv);
    return attributeDescriptions;
  }

  std::vector<VkVertexInputBindingDescription> ArkModel::Vertex::GetBindingDescriptions()
  {
    std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
    bindingDescriptions[0].binding = 0;
    bindingDescriptions[0].stride = sizeof(Vertex);
    bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    return bindingDescriptions;
  }

  void ArkModel::Builder::LoadModel(const std::string& filePath)
  {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filePath.c_str()))
    {
      throw std::runtime_error(warn + err);
    }

    vertices.clear();
    indices.clear();

    std::unordered_map<Vertex, uint32_t> uniqueVertices{};
    for (const auto& shape : shapes)
    {
      for (const auto& index : shape.mesh.indices)
      {
        Vertex vertex{};

        if (index.vertex_index >= 0)
        {
          vertex.position = {
            attrib.vertices[3 * index.vertex_index + 0], attrib.vertices[3 * index.vertex_index + 1],
            attrib.vertices[3 * index.vertex_index + 2],
          };

          vertex.color = {
            attrib.colors[3 * index.vertex_index + 0], attrib.colors[3 * index.vertex_index + 1],
            attrib.colors[3 * index.vertex_index + 2],
          };
        }

        if (index.normal_index >= 0)
        {
          vertex.normal = {
            attrib.normals[3 * index.normal_index + 0], attrib.normals[3 * index.normal_index + 1],
            attrib.normals[3 * index.normal_index + 2],
          };
        }

        if (index.texcoord_index >= 0)
        {
          vertex.uv = {
            attrib.texcoords[2 * index.texcoord_index + 0], attrib.texcoords[2 * index.texcoord_index + 1],
          };
        }
        if (uniqueVertices.count(vertex) == 0)
        {
          uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
          vertices.push_back(vertex);
        }
        indices.push_back(uniqueVertices[vertex]);
      }
    }
  }
}
