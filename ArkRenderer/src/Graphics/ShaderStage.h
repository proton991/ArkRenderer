#pragma once

#include <string>
#include <unordered_map>

namespace Graphics
{
	const std::unordered_map<std::string, int> TYPE2_GL_ENUM{
		{"vertex", GL_VERTEX_SHADER},
		{"fragment", GL_FRAGMENT_SHADER},
		{"geometry", GL_GEOMETRY_SHADER},
		{"compute", GL_COMPUTE_SHADER}
	};

	struct ShaderStage
	{
		ShaderStage() noexcept = default;

		ShaderStage(std::string& path, std::string& type) :
			m_filePath(std::move(path)), m_type(std::move(type))
		{
		}

		std::string m_filePath;
		std::string m_type;
	};
}; // namespace Graphics
