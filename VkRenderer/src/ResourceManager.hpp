#pragma once
#include <filesystem>

class ResourceManager
{
	ResourceManager() = default;
	~ResourceManager() = default;
public:
	static auto& GetInstance()
	{
		static ResourceManager instance;
		return instance;
	}

	ResourceManager(const ResourceManager&) = delete;
	ResourceManager& operator=(const ResourceManager&) = delete;
	// Load a text file and return as a string.
	std::vector<char> ReadTextFile(const std::filesystem::path& path) const;
};
