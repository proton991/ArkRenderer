#pragma once

//libs
#define GLM_FORCE_RADIANSk
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <glm/gtc/matrix_transform.hpp>

namespace Ark
{
	class ArkCamera
	{
	public:
		ArkCamera(glm::vec3 eye, glm::vec3 target, float fov, float aspect,
		          float near, float far);

		void SetOrthographicProjection(float left, float right, float top,
		                               float bottom, float near, float far);

		void SetPerspectiveProjection(float fovY, float aspect, float near,
		                              float far);

		void SetViewDirection(glm::vec3 position, glm::vec3 direction,
		                      glm::vec3 up = glm::vec3{0.f, -1.f, 0.f});

		// lock camera with a target
		void SetViewTarget(glm::vec3 position, glm::vec3 target,
		                   glm::vec3 up = glm::vec3{0.f, -1.f, 0.f});

		void SetViewYXZ(glm::vec3 position, glm::vec3 rotation);
		const glm::mat4& GetProjection() const { return m_projectionMatrix; }
		const glm::mat4& GetView() const { return m_viewMatrix; }

		void SetAspect(const float aspect)
		{
			m_aspect = aspect;
		}
		auto GetViewMatrix() const
		{
			return glm::lookAt(m_position, m_position + m_front, m_up);
		}

		// TODO: optimize projection matrix calculation
		auto GetProjMatrix() const
		{
			return glm::perspective(m_fovY, m_aspect, m_near, m_far);
		}

		void Update(const double deltaTime);

	private:
		enum class Direction
		{
			FORWARD,
			BACKWARD,
			LEFT,
			RIGHT,
			UP,
			DOWN
		};

		// Processes input received from mouse.
		void UpdateView(const bool constrainPitch = true);

		// Processes input received from keyboard.
		void ProcessKeyboard(const Direction direction,
		                     const double deltaTime) noexcept;

		// Calculates the front vector from the Camera's (updated) Eular Angles
		void UpdateVectors();

		// Camera Attributes
		glm::vec3 m_position{0.0f, 0.0f, 0.0f};
		glm::vec3 m_front;
		glm::vec3 m_up{0.0f, 1.0f, 0.0f};
		glm::vec3 m_right;
		const glm::vec3 m_worldUp{0.0f, 1.0f, 0.0f};

		float m_aspect;
		float m_fovY;

		// Eular Angles
		float m_yaw;
		float m_pitch;

		float m_near{0.1f};
		float m_far{10.0f};
		glm::mat4 m_projectionMatrix{1.f};
		glm::mat4 m_viewMatrix{1.f};

		float m_speed{5.0f};
		const float m_sensitivity{0.3f};

		// Mouse positions
		bool m_firstMouse{true};
		double m_prevX{0.0}, m_prevY{0.0};

		// Should we update the camera attributes?
		bool m_dirty{true};
	};
}
