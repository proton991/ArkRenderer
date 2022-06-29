#pragma once
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

struct Vertex
{
	using Vec2 = glm::vec2;
	using Vec3 = glm::vec3;
	Vec3 m_position;
	Vec2 m_texCoords;
	Vec3 m_normal;
	Vec3 m_tangent;
	Vertex() = default;
	Vertex(const Vec3& position, const Vec3& texCoords, const Vec3& normal, const Vec3& tangent)
		: m_position(position), m_texCoords(texCoords), m_normal(normal), m_tangent(tangent)
	{}
	Vertex(const Vec3& position, const Vec3& texCoords, const Vec3& normal)
		: m_position(position), m_texCoords(texCoords), m_normal(normal)
	{}
	Vertex(const Vec3& position, const Vec2& texCoords)
		: m_position(position), m_texCoords(texCoords)
	{}
	Vertex(const Vec3& position)
		: m_position(position)
	{}
};