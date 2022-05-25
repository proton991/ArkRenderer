#pragma once
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

struct Vertex
{
	using vec2 = glm::vec2;
	using vec3 = glm::vec3;
	vec3 m_position;
	vec3 m_texCoords;
	vec3 m_mormal;
	vec3 m_tangent;
	Vertex() = default;
	Vertex(const vec3& position, const vec3& texCoords, const vec3& normal, const vec3& tangent)
		: m_position(position), m_texCoords(texCoords), m_mormal(normal), m_tangent(tangent)
	{}
	Vertex(const vec3& position, const vec3& texCoords, const vec3& normal)
		: m_position(position), m_texCoords(texCoords), m_mormal(normal)
	{}
	Vertex(const vec3& position, const vec3& texCoords)
		: m_position(position), m_texCoords(texCoords)
	{}
};