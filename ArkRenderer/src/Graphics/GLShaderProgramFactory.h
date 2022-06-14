#pragma once

#include "GLShaderProgram.h"
#include "ShaderStage.h"

#include <optional>
#include <vector>

namespace Graphics
{
	class GLShaderProgramFactory
	{
	public:
		static std::optional<GLShaderProgram> CreateShaderProgram(
			const std::string& programName,
			const std::vector<ShaderStage>& stages
		);
	};
}; // namespace Graphics
