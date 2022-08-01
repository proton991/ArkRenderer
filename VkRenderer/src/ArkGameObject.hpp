#pragma once
#include "ArkModel.hpp"

// libs
#include <glm/gtc/matrix_transform.hpp>
#include <unordered_map>
namespace Ark
{
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

  class ArkGameObject
  {
  public:
    using IdType = unsigned int;
    using Map = std::unordered_map<IdType, ArkGameObject>;
    static ArkGameObject Create()
    {
      static IdType currentId = 0;
      return {currentId++};
    }

    IdType GetId() const { return m_id; }
    ArkGameObject(const ArkGameObject&) = delete;
    ArkGameObject& operator=(const ArkGameObject&) = delete;
    ArkGameObject(ArkGameObject&&) = default;
    ArkGameObject& operator=(ArkGameObject&&) = default;
    std::shared_ptr<ArkModel> m_model{};
    glm::vec3 m_color{};
    TransformComponent m_transform{};
  private:
    IdType m_id;

    ArkGameObject(const IdType objId) : m_id(objId)
    {
    }
  };
}
