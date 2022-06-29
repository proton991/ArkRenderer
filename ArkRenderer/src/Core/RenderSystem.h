#pragma once
#include <string>
#include "../Model.h"
#include <unordered_map>
#include "../Graphics/GLShaderProgram.h"
#include "../Graphics/GLVertexArray.h"
class Camera;

class RenderSystem
{
	using RenderListIterator = std::vector<ModelPtr>::const_iterator;
public:
	RenderSystem() = default;
	void Init();
	void Render(const Camera& camera);
private:
	// Screen-quad
	GLVertexArray m_quadVao;
	std::vector<ModelPtr> m_models;
	// Texture samplers
	GLuint m_samplerPBRTextures{ 0 };


	// Setup texture samplers
	void SetupTextureSamplers();
	void SetDefaultState();
	// Compiled shader cache
	std::unordered_map<std::string, GLShaderProgram> m_shaderCache;
	void CompileShader();
	// Configure NDC screen quad
	void SetupScreenQuad();
	void RenderQuad() const;
	// Render models without binding textures (for a depth or shadow pass perhaps)
	void RenderModelsNoTextures(GLShaderProgram& shader, RenderListIterator renderListBegin, RenderListIterator renderListEnd) const;
	// Render models contained in the renderlist
	void RenderModelsWithTextures(GLShaderProgram& shader, RenderListIterator renderListBegin, RenderListIterator renderListEnd) const;
};
