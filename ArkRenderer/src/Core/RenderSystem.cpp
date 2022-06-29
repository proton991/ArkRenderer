#include "RenderSystem.h"
#include <iostream>
#include <GLFW/glfw3.h>
#include <array>
#include "../ResourceManager.h"
#include <glm/gtx/string_cast.hpp>
#include "../Graphics/ShaderStage.h"
#include "../Vertex.h"
#include "../Graphics/GLShaderProgramFactory.h"
#include "../Model.h"
#include "../Camera.h"

void RenderSystem::SetDefaultState()
{
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
void RenderSystem::SetupTextureSamplers()
{
	// Sampler for PBR textures used on meshes
	glGenSamplers(1, &m_samplerPBRTextures);
	glSamplerParameteri(m_samplerPBRTextures, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glSamplerParameteri(m_samplerPBRTextures, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glSamplerParameteri(m_samplerPBRTextures, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glSamplerParameteri(m_samplerPBRTextures, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void RenderSystem::Init()
{
	if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
	{
		std::cerr << "Failed to start GLAD.";
		std::abort();
	}
	CompileShader();
	SetupTextureSamplers();
	SetupScreenQuad();
	auto& modelShader = m_shaderCache.at("ModelShader");
	modelShader.Bind();
	modelShader.SetUniformi("diffuseMap", 1);
	auto model = ResourceManager::GetInstance().GetModel("backpack", "resource/models/backpack/backpack.obj");
	model->Translate(glm::vec3(0.0f, 0.0f, 0.0f));
	model->Scale(glm::vec3(1.0f, 1.0f, 1.0f));
	m_models.emplace_back(model);
	

}

void RenderSystem::Render(const Camera& camera)
{
	SetDefaultState();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	auto& modelShader = m_shaderCache.at("ModelShader");
	modelShader.Bind();
	modelShader.SetUniform("view", camera.GetViewMatrix());
	modelShader.SetUniform("projection", camera.GetProjMatrix(1024, 768));
	RenderModelsWithTextures(modelShader, m_models.cbegin(), m_models.cend());
	//RenderQuad();
}

void RenderSystem::CompileShader()
{
	m_shaderCache.clear();
	std::vector<Graphics::ShaderStage> stages;
	Graphics::ShaderStage stage{
		"resource/shaders/model_loadingvs.glsl", "vertex"
	};
	stages.emplace_back(stage);
	stages.emplace_back(Graphics::ShaderStage{
		"resource/shaders/model_loadingps.glsl", "fragment"
	});
	const std::string name = "ModelShader";
	auto shaderProgram{
		Graphics::GLShaderProgramFactory::CreateShaderProgram(name, stages)
	};

	if (shaderProgram)
	{
		m_shaderCache.try_emplace(name, std::move(shaderProgram.value()));
		// value_or for default and remove if-check?
	}
}

void RenderSystem::SetupScreenQuad()
{
	const std::array<Vertex, 4> screenQuadVertices{
		// Positions				// GLTexture Coords
		Vertex({-1.0f, 1.0f, 0.0f}, {0.0f, 1.0f}),
		Vertex({-1.0f, -1.0f, 0.0f}, {0.0f, 0.0f}),
		Vertex({1.0f, 1.0f, 0.0f}, {1.0f, 1.0f}),
		Vertex({1.0f, -1.0f, 0.0f}, {1.0f, 0.0f})
	};
	m_quadVao.Init();
	m_quadVao.Bind();
	m_quadVao.AttachBuffer(GLVertexArray::BufferType::Array,
	                       sizeof(Vertex) * screenQuadVertices.size(),
	                       GLVertexArray::DrawMode::Static,
	                       screenQuadVertices.data());
	m_quadVao.EnableAttribute(0, 3, sizeof(Vertex), nullptr);
	m_quadVao.EnableAttribute(1, 2, sizeof(Vertex),
	                          reinterpret_cast<void*>(offsetof(
		                          Vertex, m_texCoords)));
}

void RenderSystem::RenderQuad() const
{
	m_quadVao.Bind();
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}


void RenderSystem::RenderModelsNoTextures(GLShaderProgram& shader,
                                          RenderListIterator renderListBegin,
                                          RenderListIterator renderListEnd)
const
{
	auto begin{renderListBegin};

	while (begin != renderListEnd)
	{
		shader.SetUniform("model", (*begin)->GetModelMatrix());
		const auto& meshes{(*begin)->GetMeshes()};
		for (const auto& mesh : meshes)
		{
			mesh.m_vao.Bind();
			glDrawElements(GL_TRIANGLES, static_cast<int>(mesh.m_indexCount),
			               GL_UNSIGNED_INT, nullptr);
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		++begin;
	}
}

void RenderSystem::RenderModelsWithTextures(GLShaderProgram& shader, RenderListIterator renderListBegin, RenderListIterator renderListEnd) const
{
	glBindSampler(m_samplerPBRTextures, 1);
	//glBindSampler(m_samplerPBRTextures, 5);
	//glBindSampler(m_samplerPBRTextures, 6);

	auto begin{ renderListBegin };

	while (begin != renderListEnd) {
		shader.SetUniform("model", (*begin)->GetModelMatrix());
		const auto& meshes{ (*begin)->GetMeshes() };
		for (const auto& mesh : meshes) {
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, mesh.Material->GetParameterTexture(PBRMaterial::ALBEDO));
			//glActiveTexture(GL_TEXTURE1);
			//glBindTexture(GL_TEXTURE_2D, mesh.Material->GetParameterTexture(PBRMaterial::NORMAL));
			//glActiveTexture(GL_TEXTURE2);
			//glBindTexture(GL_TEXTURE_2D, mesh.Material->GetParameterTexture(PBRMaterial::ROUGHNESS));

			mesh.m_vao.Bind();
			glDrawElements(GL_TRIANGLES, static_cast<int>(mesh.m_indexCount), GL_UNSIGNED_INT, nullptr);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
		
		++begin;
	}
	glBindSampler(m_samplerPBRTextures, 0);
	
}
