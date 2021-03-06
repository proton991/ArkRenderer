#pragma once

//libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
namespace Ark
{
	class ArkCamera
	{
	public:
		void SetOrthographicProjection(float left, float right, float top,
		                               float bottom, float near, float far);

		void SetPerspectiveProjection(float fovY, float aspect, float near,
		                              float far);

		void SetViewDirection(glm::vec3 position, glm::vec3 direction,
		                      glm::vec3 up = glm::vec3{0.f, -1.f, 0.f});

		// lock camera with a target
		void SetViewTarget(glm::vec3 position, glm::vec3 target,
			glm::vec3 up = glm::vec3{ 0.f, -1.f, 0.f });

		void SetViewYXZ(glm::vec3 position, glm::vec3 rotation);
		const glm::mat4& GetProjection() const { return m_projectionMatrix; }
		const glm::mat4& GetView() const { return m_viewMatrix; }

	private:
		glm::mat4 m_projectionMatrix{1.f};
		glm::mat4 m_viewMatrix{ 1.f };
	}
	;
}
