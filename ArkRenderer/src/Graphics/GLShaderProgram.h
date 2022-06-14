#pragma once
#include <glm/mat4x2.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>
#include <unordered_map>
#include <string>

class GLShaderProgram
{
private:
	std::unordered_map<std::string, int> m_uniforms;
	GLuint m_programId{0};
	std::string m_programName;

	void GetUniforms();
public:
	GLShaderProgram(const std::string& programName, const GLuint programId);
	~GLShaderProgram();

	GLShaderProgram(GLShaderProgram&& other) noexcept
	{
		m_uniforms = other.m_uniforms;
		m_programId = other.m_programId;
		m_programName = other.m_programName;

		other.m_uniforms.clear();
		other.m_programId = 0;
		other.m_programName.clear();
	}

	GLShaderProgram& operator=(GLShaderProgram&& other) noexcept
	{
		if (this != &other)
		{
			m_programId = other.m_programId;
			m_uniforms = std::move(other.m_uniforms);
			m_programName = std::move(other.m_programName);
		}
		return *this;
	}

	GLShaderProgram& operator=(GLShaderProgram other) noexcept
	{
		std::swap(m_uniforms, other.m_uniforms);
		std::swap(m_programId, other.m_programId);
		std::swap(m_programName, other.m_programName);
		return *this;
	}

	void Bind() const;
	void DeleteProgram() const;
	// Disable Copying
	GLShaderProgram(const GLShaderProgram&) = delete;
	GLShaderProgram& operator=(const GLShaderProgram&) = delete;
	GLShaderProgram& SetUniformi(const std::string& uniformName,
	                             const int value);
	GLShaderProgram& SetUniformf(const std::string& uniformName,
	                             const float value);
	GLShaderProgram& SetUniform(const std::string& uniformName,
	                            const glm::ivec2& value);
	GLShaderProgram& SetUniform(const std::string& uniformName,
	                            const glm::vec2& value);
	GLShaderProgram& SetUniform(const std::string& uniformName,
	                            const glm::vec3& value);
	GLShaderProgram& SetUniform(const std::string& uniformName,
	                            const glm::vec4& value);
	GLShaderProgram& SetUniform(const std::string& uniformName,
	                            const glm::mat3x3& value);
	GLShaderProgram& SetUniform(const std::string& uniformName,
	                            const glm::mat4x4& value);

	auto GetProgramName() const noexcept { return m_programName; }
};
