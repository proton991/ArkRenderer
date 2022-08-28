#pragma once
#include "ArkModel.hpp"
#include "Texture.hpp"

// libs
#include <glm/gtc/matrix_transform.hpp>
#include <unordered_map>

#include "ArkSwapChain.hpp"

namespace Ark
{
  class ArkGameObjectManager;

  struct TransformComponent
  {
    glm::vec3 translation{};
    glm::vec3 scale{1.f, 1.f, 1.f};
    glm::vec3 rotation{};

    // Matrix corrsponds to Translate * Ry * Rx * Rz * Scale
    // Rotations correspond to Tait-bryan angles of Y(1), X(2), Z(3)
    // https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix
    glm::mat4 Mat4();
    glm::mat3 NormalMat();
  };

  struct GameObjectBufferData {
    glm::mat4 modelMatrix{ 1.f };
    glm::mat4 normalMatrix{ 1.f };
  };

  class ArkGameObject
  {
  public:
    using IdType = unsigned int;
    using Map = std::unordered_map<IdType, ArkGameObject>;

    ArkGameObject(ArkGameObject&&) = default;
    ArkGameObject(const ArkGameObject&) = delete;
    ArkGameObject& operator=(const ArkGameObject&) = delete;
    ArkGameObject& operator=(ArkGameObject&&) = delete;

    IdType GetId() const { return m_id; }

    VkDescriptorBufferInfo GetBufferInfo(int frameIndex);

    std::shared_ptr<ArkModel> m_model{};
    glm::vec3 m_color{};
    TransformComponent m_transform{};
    std::shared_ptr<Texture> m_diffuseMap = VK_NULL_HANDLE;
  private:
    ArkGameObject(const IdType objId, const ArkGameObjectManager& manager);
    IdType m_id;
    const ArkGameObjectManager& m_gameObjectManger;
    friend class ArkGameObjectManager;
  };

  class ArkGameObjectManager
  {
  public:
    static constexpr int MAX_GAME_OBJECTS = 1000;

    ArkGameObjectManager(ArkDevice& device);
    ArkGameObjectManager(const ArkGameObjectManager&) = delete;
    ArkGameObjectManager& operator=(const ArkGameObjectManager&) = delete;
    ArkGameObjectManager(ArkGameObjectManager&&) = delete;
    ArkGameObjectManager& operator=(ArkGameObjectManager&&) = delete;

    ArkGameObject& CreateGameObject()
    {
      assert(m_currentId < MAX_GAME_OBJECTS && "Max game object count exceeded!");
      auto gameObject = ArkGameObject{ m_currentId++, *this};
      auto gameObjectId = gameObject.GetId();
      gameObject.m_diffuseMap = m_textureDefault;
      m_gameObjects.emplace(gameObjectId, std::move(gameObject));
      return m_gameObjects.at(gameObjectId);
    }


    VkDescriptorBufferInfo GetBufferInfoForGameObject(
      int frameIndex, ArkGameObject::IdType gameObjectId) const
    {
      return m_uboBuffers[frameIndex]->DescriptorInfoForIndex(gameObjectId);
    }

    void UpdateBuffer(int frameIndex);

    ArkGameObject::Map m_gameObjects{};
    std::vector<std::unique_ptr<ArkBuffer>> m_uboBuffers{ArkSwapChain::MAX_FRAMES_IN_FLIGHT};

  private:
    ArkGameObject::IdType m_currentId = 0;
    std::shared_ptr<Texture> m_textureDefault;
  };
}
