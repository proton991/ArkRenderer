#pragma once
#include <memory>
#include <string>
#include <glm/ext/matrix_float4x4.hpp>
#include "Vertex.h"
#include "Mesh.h"
#include "AABB.h"
struct aiScene;
struct aiNode;
struct aiMesh;

class Model
{
public:
	Model() = default;
	Model(const std::string_view path, const std::string_view name,
		const bool flipWindingOrder, const bool loadMaterial);
	Model(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
	virtual ~Model() = default;

	[[nodiscard]] auto GetMeshes() const noexcept { return m_meshes; }
	[[nodiscard]] auto GetBoundingBox() const noexcept { return m_aabb; }
	void AttachMesh(const Mesh mesh) noexcept;

	// Transformations
	void Scale(const glm::vec3& scale);
	void Rotate(const float radians, const glm::vec3& axis);
	void Translate(const glm::vec3& pos);
	[[nodiscard]] glm::mat4 GetModelMatrix() const;
	void Delete(); // Called by ResourceManager
private:
	bool LoadModel(std::string_view path, bool flipWindingOrder, bool loadMaterial);
	void ProcessNode(aiNode* node, const aiScene* scene, bool loadMaterial);
	Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene, bool loadMaterial);
	// Transformation data
	glm::vec3 m_scale, m_position, m_axis;
	float m_radians;
	AABB m_aabb;
	std::vector<Mesh> m_meshes;
	// Model name
	const std::string m_name;
	// Location on disk holding model and textures
	std::string m_path;

	std::size_t m_numMats;
};
using ModelPtr = std::shared_ptr<Model>;
