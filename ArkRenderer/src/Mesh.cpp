#include "Mesh.h"

#include <iostream>

Mesh::Mesh(const std::vector<Vertex>& vertices,
           const std::vector<unsigned>& indices) : m_indexCount(indices.size())
{
	SetUp(vertices, indices);
}

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices, const PBRMaterialPtr& material) :
	m_indexCount(indices.size()),
	Material(material) {

	SetUp(vertices, indices);
}

void Mesh::SetUp(const std::vector<Vertex>& vertices,
                 const std::vector<unsigned int>& indices)
{
	m_vao.Init();
	m_vao.Bind();
	m_vao.AttachBuffer(GLVertexArray::Array, vertices.size() * sizeof(Vertex),
	                   GLVertexArray::DrawMode::Static, &vertices[0]);
	m_vao.AttachBuffer(GLVertexArray::Element,
	                   indices.size() * sizeof(unsigned int),
	                   GLVertexArray::DrawMode::Static, &indices[0]);
	constexpr auto vertexSize = sizeof(Vertex);
	m_vao.EnableAttribute(0, 3, vertexSize, nullptr);
	m_vao.EnableAttribute(1, 2, vertexSize, reinterpret_cast<void*>(offsetof(Vertex, m_texCoords)));
	m_vao.EnableAttribute(2, 3, vertexSize, reinterpret_cast<void*>(offsetof(Vertex, m_normal)));
	m_vao.EnableAttribute(3, 3, vertexSize, reinterpret_cast<void*>(offsetof(Vertex, m_tangent)));
}