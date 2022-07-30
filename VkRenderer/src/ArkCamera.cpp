#include "ArkCamera.hpp"

//std
#include <cassert>
#include <limits>

#include "InputController.hpp"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace Ark
{
  ArkCamera::ArkCamera(glm::vec3 eye, glm::vec3 target, float fov, float aspect, float near, float far) :
    m_position(eye), m_aspect(aspect), m_fovY(fov), m_near(near), m_far(far)
  {
    glm::vec3 dir = glm::normalize(target - m_position);
    m_pitch = glm::degrees(asin(dir.y));
    m_yaw = glm::degrees(atan2(dir.z, dir.x));
    //m_yaw = -90.0f;
    //m_pitch = 0.0f;
    UpdateVectors();
    SetPerspectiveProjection();
  }

  void ArkCamera::SetOrthographicProjection(float left, float right, float top, float bottom, float near,
                                            float far)
  {
    m_projectionMatrix = glm::mat4{1.0f};
    m_projectionMatrix[0][0] = 2.f / (right - left);
    m_projectionMatrix[1][1] = 2.f / (bottom - top);
    m_projectionMatrix[2][2] = 1.f / (far - near);
    m_projectionMatrix[3][0] = -(right + left) / (right - left);
    m_projectionMatrix[3][1] = -(bottom + top) / (bottom - top);
    m_projectionMatrix[3][2] = -near / (far - near);
  }

  void ArkCamera::SetPerspectiveProjection()
  {
    assert(glm::abs(m_aspect - std::numeric_limits<float>::epsilon()) > 0.0f);
    m_projectionMatrix = glm::perspective(m_fovY, m_aspect, m_near, m_far);
    m_projectionMatrix[1][1] *= -1;
  }

  void ArkCamera::SetViewDirection(glm::vec3 position, glm::vec3 direction, glm::vec3 up)
  {
    const glm::vec3 w{glm::normalize(direction)};
    const glm::vec3 u{glm::normalize(glm::cross(w, up))};
    const glm::vec3 v{glm::cross(w, u)};

    m_viewMatrix = glm::mat4{1.f};
    m_viewMatrix[0][0] = u.x;
    m_viewMatrix[1][0] = u.y;
    m_viewMatrix[2][0] = u.z;
    m_viewMatrix[0][1] = v.x;
    m_viewMatrix[1][1] = v.y;
    m_viewMatrix[2][1] = v.z;
    m_viewMatrix[0][2] = w.x;
    m_viewMatrix[1][2] = w.y;
    m_viewMatrix[2][2] = w.z;
    m_viewMatrix[3][0] = -glm::dot(u, position);
    m_viewMatrix[3][1] = -glm::dot(v, position);
    m_viewMatrix[3][2] = -glm::dot(w, position);
  }

  void ArkCamera::SetViewTarget(glm::vec3 position, glm::vec3 target, glm::vec3 up)
  {
    SetViewDirection(position, target - position, up);
  }

  void ArkCamera::SetViewYXZ(glm::vec3 position, glm::vec3 rotation)
  {
    const float c3 = glm::cos(rotation.z);
    const float s3 = glm::sin(rotation.z);
    const float c2 = glm::cos(rotation.x);
    const float s2 = glm::sin(rotation.x);
    const float c1 = glm::cos(rotation.y);
    const float s1 = glm::sin(rotation.y);
    const glm::vec3 u{(c1 * c3 + s1 * s2 * s3), (c2 * s3), (c1 * s2 * s3 - c3 * s1)};
    const glm::vec3 v{(c3 * s1 * s2 - c1 * s3), (c2 * c3), (c1 * c3 * s2 + s1 * s3)};
    const glm::vec3 w{(c2 * s1), (-s2), (c1 * c2)};
    m_viewMatrix = glm::mat4{1.f};
    m_viewMatrix[0][0] = u.x;
    m_viewMatrix[1][0] = u.y;
    m_viewMatrix[2][0] = u.z;
    m_viewMatrix[0][1] = v.x;
    m_viewMatrix[1][1] = v.y;
    m_viewMatrix[2][1] = v.z;
    m_viewMatrix[0][2] = w.x;
    m_viewMatrix[1][2] = w.y;
    m_viewMatrix[2][2] = w.z;
    m_viewMatrix[3][0] = -glm::dot(u, position);
    m_viewMatrix[3][1] = -glm::dot(v, position);
    m_viewMatrix[3][2] = -glm::dot(w, position);
  }

  void ArkCamera::ProcessKeyboard(const Direction direction, const double deltaTime) noexcept
  {
    const float velocity = m_speed * static_cast<float>(deltaTime);

    switch (direction)
    {
    case Direction::FORWARD: m_position += m_front * velocity;
      break;
    case Direction::BACKWARD: m_position -= m_front * velocity;
      break;
    case Direction::LEFT: m_position -= m_right * velocity;
      break;
    case Direction::RIGHT: m_position += m_right * velocity;
      break;
    case Direction::UP: m_position += m_worldUp * velocity;
      break;
    case Direction::DOWN: m_position -= m_worldUp * velocity;
      break;
    }
  }

  void ArkCamera::UpdateVectors()
  {
    // Calculate the new Front vector
    glm::vec3 front{
      front.x = glm::cos(glm::radians(m_yaw)) * glm::cos(glm::radians(m_pitch)),
      front.y = glm::sin(glm::radians(m_pitch)),
      front.z = glm::sin(glm::radians(m_yaw)) * glm::cos(glm::radians(m_pitch))
    };

    m_front = glm::normalize(front);
    m_right = glm::normalize(glm::cross(m_front, m_worldUp));
    // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    m_up = glm::normalize(glm::cross(m_right, m_front));
  }

  void ArkCamera::Update(const double deltaTime)
  {
    if (InputManager::GetInstance().IsKeyPressed(GLFW_KEY_TAB))
    {
      m_dirty = !m_dirty;
    }
    if (m_dirty)
    {
      // Update view from mouse movement
      UpdateView();

      // Update Keyboard
      if (InputManager::GetInstance().IsKeyHeld(GLFW_KEY_W))
      {
        ProcessKeyboard(Direction::FORWARD, deltaTime);
      }
      if (InputManager::GetInstance().IsKeyHeld(GLFW_KEY_S))
      {
        ProcessKeyboard(Direction::BACKWARD, deltaTime);
      }
      if (InputManager::GetInstance().IsKeyHeld(GLFW_KEY_A))
      {
        ProcessKeyboard(Direction::LEFT, deltaTime);
      }
      if (InputManager::GetInstance().IsKeyHeld(GLFW_KEY_D))
      {
        ProcessKeyboard(Direction::RIGHT, deltaTime);
      }
      if (InputManager::GetInstance().IsKeyHeld(GLFW_KEY_SPACE))
      {
        ProcessKeyboard(Direction::UP, deltaTime);
      }
      if (InputManager::GetInstance().IsKeyHeld(GLFW_KEY_LEFT_CONTROL))
      {
        ProcessKeyboard(Direction::DOWN, deltaTime);
      }
    }
  }

  void ArkCamera::UpdateView(const bool constrainPitch)
  {
    // If the mouse position has changed, recalculate vectors
    if (InputManager::GetInstance().MouseMoved())
    {
      const auto xPos = InputManager::GetInstance().GetMouseX();
      const auto yPos = InputManager::GetInstance().GetMouseY();

      if (m_firstMouse)
      {
        m_prevX = xPos;
        m_prevY = yPos;
        m_firstMouse = false;
      }
      const auto xOffset = -1 * (xPos - m_prevX) * m_sensitivity;
      //const auto yOffset = (yPos - m_prevY) * m_sensitivity;
      const auto yOffset = -1 * (m_prevY - yPos) * m_sensitivity;
      // Reversed since y-coordinates go from bottom to top

      m_prevX = xPos;
      m_prevY = yPos;

      m_yaw += xOffset;
      m_pitch += yOffset;

      // Make sure that when pitch is out of bounds, screen doesn't get flipped
      if (constrainPitch)
      {
        if (m_pitch > 89.0f) { m_pitch = 89.0f; }
        if (m_pitch < -89.0f) { m_pitch = -89.0f; }
      }

      // Update Front, Right and Up Vectors using the updated Eular angles
      UpdateVectors();
    }
  }
}
