#include "ArkGameObject.hpp"

#include <numeric>

namespace Ark
{
  glm::mat4 TransformComponent::Mat4()
  {
    const float c3 = glm::cos(rotation.z);
    const float s3 = glm::sin(rotation.z);
    const float c2 = glm::cos(rotation.x);
    const float s2 = glm::sin(rotation.x);
    const float c1 = glm::cos(rotation.y);
    const float s1 = glm::sin(rotation.y);
    return glm::mat4{
      {scale.x * (c1 * c3 + s1 * s2 * s3), scale.x * (c2 * s3), scale.x * (c1 * s2 * s3 - c3 * s1), 0.0f,},
      {scale.y * (c3 * s1 * s2 - c1 * s3), scale.y * (c2 * c3), scale.y * (c1 * c3 * s2 + s1 * s3), 0.0f,},
      {scale.z * (c2 * s1), scale.z * (-s2), scale.z * (c1 * c2), 0.0f,},
      {translation.x, translation.y, translation.z, 1.0f}
    };
  }

  glm::mat3 TransformComponent::NormalMat()
  {
    const float c3 = glm::cos(rotation.z);
    const float s3 = glm::sin(rotation.z);
    const float c2 = glm::cos(rotation.x);
    const float s2 = glm::sin(rotation.x);
    const float c1 = glm::cos(rotation.y);
    const float s1 = glm::sin(rotation.y);
    const glm::vec3 invScale = 1.0f / scale;
    return glm::mat3{
      {invScale.x * (c1 * c3 + s1 * s2 * s3), scale.x * (c2 * s3), scale.x * (c1 * s2 * s3 - c3 * s1)},
      {invScale.y * (c3 * s1 * s2 - c1 * s3), scale.y * (c2 * c3), scale.y * (c1 * c3 * s2 + s1 * s3)},
      {invScale.z * (c2 * s1), scale.z * (-s2), scale.z * (c1 * c2)}
    };
  }


  ArkGameObjectManager::ArkGameObjectManager(ArkDevice& device) {
    // including nonCoherentAtomSize allows us to flush a specific index at once
    int alignment = std::lcm(
      device.properties.limits.nonCoherentAtomSize,
      device.properties.limits.minUniformBufferOffsetAlignment);
    for (uint32_t i = 0; i < m_uboBuffers.size(); i++) {
      m_uboBuffers[i] = std::make_unique<ArkBuffer>(
        device,
        sizeof(GameObjectBufferData),
        ArkGameObjectManager::MAX_GAME_OBJECTS,
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
        alignment);
      m_uboBuffers[i]->Map();
    }

    m_textureDefault = Texture::CreateTextureFromFile(device, "textures/missing.png");
  }

  void ArkGameObjectManager::UpdateBuffer(int frameIndex) {
    // copy model matrix and normal matrix for each gameObj into
    // buffer for this frame
    for (auto& kv : m_gameObjects) {
      auto& obj = kv.second;
      GameObjectBufferData data{};
      data.modelMatrix = obj.m_transform.Mat4();
      data.normalMatrix = obj.m_transform.NormalMat();
      m_uboBuffers[frameIndex]->WriteToIndex(&data, kv.first);
    }
    m_uboBuffers[frameIndex]->Flush();
  }

  VkDescriptorBufferInfo ArkGameObject::GetBufferInfo(int frameIndex) {
    return m_gameObjectManger.GetBufferInfoForGameObject(frameIndex, m_id);
  }

  ArkGameObject::ArkGameObject(IdType objId, const ArkGameObjectManager& manager)
    : m_id{ objId }, m_gameObjectManger{ manager } {}
}
