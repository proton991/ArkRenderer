#pragma once
#include "Graphics/GLVertexArray.h"
#include "Vertex.h"
#include <vector>
#include "PBRMaterial.h"

struct Mesh
{
public:
	GLVertexArray m_vao;
	const std::size_t m_indexCount;
	PBRMaterialPtr Material;
	Mesh(const std::vector<Vertex>& vertices,
	     const std::vector<unsigned int>& indices);
	Mesh(const std::vector<Vertex>& vertices,
	     const std::vector<GLuint>& indices, const PBRMaterialPtr& material);

	[[nodiscard]] auto GetTriangleCount() const noexcept
	{
		return m_indexCount / 3;
	}

	void Clear();
private:
	void SetUp(const std::vector<Vertex>& vertices,
	           const std::vector<unsigned int>& indices);
};
