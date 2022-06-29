#include "Model.h"

#include <iostream>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <glm/ext/matrix_transform.hpp>

#include "ResourceManager.h"

Model::Model(const std::string_view path, const std::string_view name, const bool flipWindingOrder, const bool loadMaterial)
{
	if (!LoadModel(path, flipWindingOrder, loadMaterial))
	{
		std::cerr << "Failed to load: " << name << '\n';
	}
	else
	{
		std::cout << "Loaded " << name << " model successfully!" << " model folder path: " << m_path << "\n";
		std::cout << "Mesh size: " << m_meshes.size() << "\n";
	}
}
Model::Model(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices)
{
	m_meshes.emplace_back(vertices, indices);
}

bool Model::LoadModel(std::string_view path, bool flipWindingOrder, bool loadMaterial)
{
	Assimp::Importer importer;
	const aiScene* scene = nullptr;
	scene = importer.ReadFile(path.data(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
	//if (flipWindingOrder) {
	//	scene = importer.ReadFile(path.data(), aiProcess_Triangulate |
	//		aiProcess_JoinIdenticalVertices |
	//		aiProcess_GenUVCoords |
	//		aiProcess_SortByPType |
	//		aiProcess_RemoveRedundantMaterials |
	//		aiProcess_FindInvalidData |
	//		aiProcess_FlipUVs |
	//		aiProcess_FlipWindingOrder | // Reverse back-face culling
	//		aiProcess_CalcTangentSpace |
	//		aiProcess_OptimizeMeshes |
	//		aiProcess_SplitLargeMeshes);
	//}
	//else {
	//	scene = importer.ReadFile(path.data(), aiProcess_Triangulate |
	//		aiProcess_JoinIdenticalVertices |
	//		aiProcess_GenUVCoords |
	//		aiProcess_SortByPType |
	//		aiProcess_RemoveRedundantMaterials |
	//		aiProcess_FindInvalidData |
	//		aiProcess_FlipUVs |
	//		aiProcess_CalcTangentSpace |
	//		aiProcess_GenSmoothNormals |
	//		aiProcess_ImproveCacheLocality |
	//		aiProcess_OptimizeMeshes |
	//		aiProcess_SplitLargeMeshes);
	//}
	// Check loading model errors
	if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cerr << "[Assimp Error]:: " << m_name << ": " << importer.GetErrorString() << "\n";
		importer.FreeScene();
		return false;
	}
	m_path = path.substr(0, path.find_last_of('/'));
	m_path += "/";
	ProcessNode(scene->mRootNode, scene, loadMaterial);
	importer.FreeScene();
	return true;
}

void Model::ProcessNode(aiNode* node, const aiScene* scene, bool loadMaterial)
{
	for (auto i = 0; i < node->mNumMeshes; ++i)
	{
		auto* mesh = scene->mMeshes[node->mMeshes[i]];
		m_meshes.push_back(ProcessMesh(mesh, scene, loadMaterial));
	}
	for (auto i = 0; i < node->mNumChildren; i++)
	{
		ProcessNode(node->mChildren[i], scene, loadMaterial);
	}
}

Mesh Model::ProcessMesh(aiMesh* mesh, const aiScene* scene, bool loadMaterial)
{
	std::vector<Vertex> vertices;
	constexpr float minFloat = std::numeric_limits<float>::min();
	constexpr float maxFloat = std::numeric_limits<float>::max();
	glm::vec3 min(maxFloat, maxFloat, maxFloat);
	glm::vec3 max = {minFloat, minFloat, minFloat};
	for (auto i = 0; i < mesh->mNumVertices; ++i) {
		Vertex vertex;

		if (mesh->HasPositions()) {
			vertex.m_position.x = mesh->mVertices[i].x;
			vertex.m_position.y = mesh->mVertices[i].y;
			vertex.m_position.z = mesh->mVertices[i].z;

			// Construct bounding box
			if (vertex.m_position.x < min.x) min.x = vertex.m_position.x;
			if (vertex.m_position.x > max.x) max.x = vertex.m_position.x;

			if (vertex.m_position.y < min.y) min.y = vertex.m_position.y;
			if (vertex.m_position.y > max.y) max.y = vertex.m_position.y;

			if (vertex.m_position.z < min.z) min.z = vertex.m_position.z;
			if (vertex.m_position.z > max.z) max.z = vertex.m_position.z;

		}

		if (mesh->HasNormals()) {
			vertex.m_normal.x = mesh->mNormals[i].x;
			vertex.m_normal.y = mesh->mNormals[i].y;
			vertex.m_normal.z = mesh->mNormals[i].z;
		}

		if (mesh->HasTangentsAndBitangents()) {
			vertex.m_tangent.x = mesh->mTangents[i].x;
			vertex.m_tangent.y = mesh->mTangents[i].y;
			vertex.m_tangent.z = mesh->mTangents[i].z;
		}

		if (mesh->HasTextureCoords(0) && loadMaterial) {
			// Just take the first set of texture coords (since we could have up to 8)
			vertex.m_texCoords.x = mesh->mTextureCoords[0][i].x;
			vertex.m_texCoords.y = mesh->mTextureCoords[0][i].y;
			
		}
		else {
			vertex.m_texCoords = glm::vec2(0.0f);
		}

		vertices.push_back(vertex);
	}
	m_aabb.Extend(min);
	m_aabb.Extend(max);

	std::vector<unsigned int> indices;
	for (auto i = 0; i < mesh->mNumFaces; i++)
	{
		const auto face = mesh->mFaces[i];
		for (auto j = 0; j < face.mNumIndices; j++)
		{
			indices.emplace_back(face.mIndices[j]);
		}
	}
	if (loadMaterial) {
		if (mesh->mMaterialIndex >= 0) {
			const auto* mat = scene->mMaterials[mesh->mMaterialIndex];

			aiString name;
			mat->Get(AI_MATKEY_NAME, name);
			// Is the material cached?
			const auto cachedMaterial = ResourceManager::GetInstance().GetMaterial(name.C_Str());
			if (cachedMaterial.has_value()) {
				return Mesh(vertices, indices, cachedMaterial.value());
			}

			// Get the first texture for each texture type we need
			// since there could be multiple textures per type
			aiString albedoPath;
			mat->GetTexture(aiTextureType_DIFFUSE, 0, &albedoPath);
			aiString metallicPath;
			mat->GetTexture(aiTextureType_AMBIENT, 0, &metallicPath);
			aiString normalPath;
			mat->GetTexture(aiTextureType_HEIGHT, 0, &normalPath);
			aiString roughnessPath;
			mat->GetTexture(aiTextureType_SHININESS, 0, &roughnessPath);
			aiString alphaMaskPath;
			mat->GetTexture(aiTextureType_OPACITY, 0, &alphaMaskPath);
			aiString aoPath;
			mat->GetTexture(aiTextureType_LIGHTMAP, 0, &aoPath);
			std::cout << "Ambient Opacity Path: " << aoPath.C_Str() << "\n";
			const auto newMaterial = ResourceManager::GetInstance().CacheMaterial(name.C_Str(),
				m_path + albedoPath.C_Str(),
				"",
				m_path + metallicPath.C_Str(),
				m_path + normalPath.C_Str(),
				m_path + roughnessPath.C_Str(),
				m_path + alphaMaskPath.C_Str());

			++m_numMats;
			return Mesh(vertices, indices, newMaterial);
		}
	}
	return Mesh(vertices, indices);
}
void Model::AttachMesh(const Mesh mesh) noexcept
{
	m_meshes.push_back(mesh);
}

void Model::Delete()
{
	for (auto& mesh : m_meshes)
	{
		mesh.m_vao.Delete();
	}
}


void Model::Scale(const glm::vec3& scale) {
	m_scale = scale;
	m_aabb.Scale(scale, glm::vec3(0.0f));
}


void Model::Rotate(const float radians, const glm::vec3& axis) {
	m_radians = radians;
	m_axis = axis;
}


void Model::Translate(const glm::vec3& pos) {
	m_position = pos;
	m_aabb.Translate(pos);
}
glm::mat4 Model::GetModelMatrix() const {
	const auto scale = glm::scale(glm::mat4(1.0f), m_scale);
	const auto translate = glm::translate(glm::mat4(1.0f), m_position);
	return scale * translate;
}