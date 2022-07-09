#pragma once
#include "ArkModel.hpp"

namespace Ark
{
	struct Transform2d
	{
		glm::vec2 translation{};
		glm::vec2 scale{.1f, .1f};
		float rotation;

		glm::mat2 Mat2()
		{
			const float s = glm::sin(glm::radians(rotation));
			const float c = glm::cos(glm::radians(rotation));
			glm::mat2 rotMat{{c, s}, {-s, c}};
			glm::mat2 scaleMat{{scale.x, .0f}, {.0f, scale.y}};
			return rotMat * scaleMat;
		}
	};

	class ArkGameObject
	{
	public:
		using IdType = unsigned int;

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
		Transform2d m_transform2d;
	private:
		IdType m_id;

		ArkGameObject(const IdType objId) : m_id(objId)
		{
		}
	};
}
