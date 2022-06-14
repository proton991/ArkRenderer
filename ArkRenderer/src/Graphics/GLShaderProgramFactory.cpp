#include "GLShaderProgramFactory.h"

#include <glad/glad.h>

#include <fmt/core.h>
#include <vector>
#include <iostream>
#include <unordered_map>

namespace Graphics
{
	bool CheckShaderError(const unsigned int id, const std::string& type)
	{
		int success;
		if (type != "PROGRAM")
		{
			int logLength;
			glGetShaderiv(id, GL_COMPILE_STATUS, &success);
			glGetShaderiv(id, GL_INFO_LOG_LENGTH, &logLength);
			std::vector<char> infoLog(logLength, 0);
			glGetShaderInfoLog(id, logLength, nullptr, infoLog.data());
			std::cout << fmt::format("[ERROR]::{} SHADER COMPILATION ERROR: {}",
			                         type, std::string(
				                         infoLog.cbegin(), infoLog.cend())) <<
				std::endl;
		}
		else
		{
			int logLength;
			glGetProgramiv(id, GL_COMPILE_STATUS, &success);
			glGetProgramiv(id, GL_INFO_LOG_LENGTH, &logLength);
			std::vector<char> infoLog(logLength, 0);
			glGetProgramInfoLog(id, logLength, nullptr, infoLog.data());
			std::cout << "[ERROR]::PROGRAM LINKING ERROR: " << fmt::format(
					"{}", std::string(infoLog.cbegin(), infoLog.cend())) <<
				std::endl;
		}

		return success == GL_TRUE;
	}

	void Compile(const unsigned int id, const char* shaderCode)
	{
		glShaderSource(id, 1, &shaderCode, nullptr);
		glCompileShader(id);
	}

	bool CompileStage(const unsigned int id, const std::string& shaderCode,
	                  const std::string& type)
	{
		Compile(id, shaderCode.c_str());
		return CheckShaderError(id, type);
	}

	bool LinkProgram(const unsigned int id)
	{
		glLinkProgram(id);
		return CheckShaderError(id, "PROGRAM");
	}

	std::optional<GLShaderProgram> GLShaderProgramFactory::CreateShaderProgram(
		const std::string& programName, const std::vector<ShaderStage>& stages)
	{
		std::cout << "Building shader program " << programName << std::endl;
		std::vector<unsigned int> shaderIds;
		bool success = true;
		for (auto& stage : stages)
		{
			auto id = glCreateShader(TYPE2_GL_ENUM.at(stage.m_type));
			shaderIds.push_back(id);
			auto shaderCode = "";
			if (!CompileStage(id, shaderCode, stage.m_type))
			{
				success = false;
				break;
			}
		}
		if (!success)
		{
			for (const auto id : shaderIds)
			{
				glDeleteShader(id);
			}
			return std::nullopt;
		}
		const unsigned int programId = glCreateProgram();
		for (const auto id : shaderIds)
		{
			glAttachShader(programId, id);
		}
		if (!LinkProgram(programId))
		{
			for (const auto id : shaderIds)
			{
				glDetachShader(programId, id);
				glDeleteShader(id);
			}
			glDeleteProgram(programId);
			return std::nullopt;
		}
		for (const auto id : shaderIds) {
			glDetachShader(programId, id);
			glDeleteShader(id);
		}
		return std::make_optional<GLShaderProgram>({ programName, programId });
	}
}
