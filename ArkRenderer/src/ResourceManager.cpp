#include "ResourceManager.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <cassert>
#include <string_view>

#include <stb_image.h>

const static std::filesystem::path COMPRESSED_TEX_DIR{ std::filesystem::current_path() / "resource/cache/textures" };

using ImageBuffer = std::unique_ptr<unsigned char[]>;

struct CompressedImageDesc {
	GLint width{ -1 };
	GLint height{ -1 };
	GLint size{ -1 };
	GLint format{ -1 };
	ImageBuffer data;
};

/***********************************************************************************/
void ResourceManager::ReleaseAllResources() {
	// Delete cached meshes
	for (auto& model : m_modelCache) {
		model.second->Delete();
	}

	// Deletes textures
	for (auto& tex : m_textureCache) {
		glDeleteTextures(1, &tex.second);
	}
}

/***********************************************************************************/
std::string ResourceManager::LoadTextFile(const std::filesystem::path& path) const {
	std::ifstream in(path, std::ios::in);
	in.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	std::cout << path.string() << "\n";
	if (!in) {
		std::cerr << "Resource Manager: File loading error: " + path.string() << " " << errno << std::endl;
		std::abort();
	}

	return std::string(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>());
}

/***********************************************************************************/
unsigned int ResourceManager::LoadHDRI(const std::string_view path) const {
	//stbi_set_flip_vertically_on_load(true);

	// Dont flip HDR otherwise the probe will be upside down. We flip the y-coord in the
	// shader to correctly render the texture.
	int width, height, nrComp;
	auto* data{ stbi_load(path.data(), &width, &height, &nrComp, 0) };

	stbi_set_flip_vertically_on_load(false);

	if (!data) {
		std::cerr << "Resource Manager: Failed to load HDRI." << std::endl;
		std::abort();
	}

	unsigned int hdrTexture{ 0 };
	glGenTextures(1, &hdrTexture);
	glBindTexture(GL_TEXTURE_2D, hdrTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	stbi_image_free(data);

#ifdef _DEBUG
	std::cout << "Resource Manager: Loaded HDR: " << path << std::endl;
#endif

	return hdrTexture;
}

/***********************************************************************************/
auto buildTextureCachePath(const std::filesystem::path& filenameNoExt) {
	const std::filesystem::path filename{ filenameNoExt.string() + ".bin" };
	return std::filesystem::path(COMPRESSED_TEX_DIR / filename);
}

/***********************************************************************************/
void saveCompressedImageToDisk(const std::filesystem::path& target, const CompressedImageDesc& desc) {
	if (!std::filesystem::exists(COMPRESSED_TEX_DIR)) {
		if (!std::filesystem::create_directories(COMPRESSED_TEX_DIR)) {
			std::cerr << "Failed to create texture cache directory: " << COMPRESSED_TEX_DIR << '\n';
			return;
		}
	}

	std::ofstream out(target, std::ios::binary);
	if (out) {
		out.write((char*)(&desc.size), sizeof(CompressedImageDesc::size));
		out.write((char*)(&desc.width), sizeof(CompressedImageDesc::width));
		out.write((char*)(&desc.height), sizeof(CompressedImageDesc::height));
		out.write((char*)(&desc.format), sizeof(CompressedImageDesc::format));
		out.write((char*)(desc.data.get()), desc.size);
	}
}

/***********************************************************************************/
std::optional<CompressedImageDesc> loadCompressedImageFromDisk(const std::filesystem::path& target) {
	CompressedImageDesc desc;

	std::ifstream in(target, std::ios::binary);
	if (!in) {
		return std::nullopt;
	}

	in.read(reinterpret_cast<char*>(&desc.size), sizeof(CompressedImageDesc::size));
	in.read(reinterpret_cast<char*>(&desc.width), sizeof(CompressedImageDesc::width));
	in.read(reinterpret_cast<char*>(&desc.height), sizeof(CompressedImageDesc::height));
	in.read(reinterpret_cast<char*>(&desc.format), sizeof(CompressedImageDesc::format));

	desc.data = std::make_unique<unsigned char[]>(desc.size);
	in.read(reinterpret_cast<char*>(desc.data.get()), desc.size);

	return std::make_optional(std::move(desc));
}
unsigned int ResourceManager::LoadTexture(const std::filesystem::path& path, const bool useMipMaps, const bool useUnalignedUnpack) {

	if (path.filename().empty()) {
		return 0;
	}
	stbi_set_flip_vertically_on_load(true);
	unsigned int textureID;
	glGenTextures(1, &textureID);
	int width = 0, height = 0, nrComponents = 0;
	std::cout << "Path to load " << path.string() << std::endl;
	unsigned char* data = stbi_load(path.string().c_str(), &width, &height, &nrComponents, 0);
	if (!data) {
		std::cerr << "Failed to load texture: " << path << std::endl;
		glDeleteTextures(1, &textureID);
		stbi_image_free(data);
		return 0;
	}

	GLenum format = 0;
	GLenum internalFormat = 0;
	switch (nrComponents) {
	case 1:
		format = GL_RED;
		internalFormat = GL_COMPRESSED_RED;
		break;
	case 3:
		format = GL_RGB;
		internalFormat = GL_COMPRESSED_RGB;
		break;
	case 4:
		format = GL_RGBA;
		internalFormat = GL_COMPRESSED_RGBA;
		break;
	}
	glBindTexture(GL_TEXTURE_2D, textureID);

	//glHint(GL_TEXTURE_COMPRESSION_HINT, GL_DONT_CARE);
	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	stbi_image_free(data);
	return m_textureCache.try_emplace(path.string(), textureID).first->second;
}

/***********************************************************************************/
//unsigned int ResourceManager::LoadTexture(const std::filesystem::path& path, const bool useMipMaps, const bool useUnalignedUnpack) {
//
//	if (path.filename().empty()) {
//		return 0;
//	}
//
//	const auto compressedFilePath{ buildTextureCachePath(path.stem()) };
//	const auto compressedImageExists{ std::filesystem::exists(compressedFilePath) };
//	const std::filesystem::path pathToLoad = compressedImageExists ? compressedFilePath : path;
//
//	// Check if texture is already loaded in memory
//	const auto val = m_textureCache.find(pathToLoad.string());
//	if (val != m_textureCache.end()) {
//		// Found it
//		return val->second;
//	}
//
//	if (useUnalignedUnpack) {
//		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
//	}
//
//	unsigned int textureID;
//	glGenTextures(1, &textureID);
//	
//	if (!compressedImageExists) {
//
//		const auto desc{ loadCompressedImageFromDisk(compressedFilePath) };
//		if (!desc) {
//			return 0;
//		}
//
//		glBindTexture(GL_TEXTURE_2D, textureID);
//		glCompressedTexImage2D(
//			GL_TEXTURE_2D,
//			0,
//			desc.value().format,
//			desc.value().width,
//			desc.value().height,
//			0,
//			desc.value().size,
//			desc.value().data.get()
//		);
//
//	}
//	else {
//		int width = 0, height = 0, nrComponents = 0;
//		std::cout << "Path to load " << pathToLoad.string() << std::endl;
//		unsigned char* data = stbi_load(pathToLoad.string().c_str(), &width, &height, &nrComponents, 0);
//		if (!data) {
//			std::cerr << "Failed to load texture: " << path << std::endl;
//			glDeleteTextures(1, &textureID);
//			stbi_image_free(data);
//			return 0;
//		}
//
//		GLenum format = 0;
//		GLenum internalFormat = 0;
//		switch (nrComponents) {
//		case 1:
//			format = GL_RED;
//			internalFormat = GL_COMPRESSED_RED;
//			break;
//		case 3:
//			format = GL_RGB;
//			internalFormat = GL_COMPRESSED_RGB;
//			break;
//		case 4:
//			format = GL_RGBA;
//			internalFormat = GL_COMPRESSED_RGBA;
//			break;
//		}
//		std::cout << "nrComponents: " << nrComponents << "\n";
//		glBindTexture(GL_TEXTURE_2D, textureID);
//
//		//glHint(GL_TEXTURE_COMPRESSION_HINT, GL_DONT_CARE);
//		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
//
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//		stbi_image_free(data);
//
//		GLint compressed = GL_FALSE;
//		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_COMPRESSED, &compressed);
//		if (compressed == GL_TRUE) {
//			CompressedImageDesc desc;
//			desc.height = height;
//			desc.width = width;
//			glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_COMPRESSED_IMAGE_SIZE, &desc.size);
//			glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &desc.format);
//
//			desc.data = std::make_unique<unsigned char[]>(desc.size);
//			glGetCompressedTexImage(GL_TEXTURE_2D, 0, (GLvoid*)desc.data.get());
//
//			saveCompressedImageToDisk(compressedFilePath, desc);
//		}
//	}
//
//	if (useMipMaps) {
//		glGenerateMipmap(GL_TEXTURE_2D);
//	}
//
//	if (useUnalignedUnpack) {
//		glPixelStorei(GL_UNPACK_ALIGNMENT, 0);
//	}
//	return m_textureCache.try_emplace(path.string(), textureID).first->second;
//}

/***********************************************************************************/
std::vector<char> ResourceManager::LoadBinaryFile(const std::string_view path) const {
	std::ifstream in(path.data(), std::ios::binary);
	in.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	if (!in) {
		std::cerr << "Resource Manager: Failed to load binary file: " << path << std::endl;
		std::abort();
	}

#ifdef _DEBUG
	std::cout << "Resource Manager: loaded binary file: " << path << std::endl;
#endif

	// Copy and return binary contents into vector
	return std::vector<char>(std::istreambuf_iterator<char>(in), (std::istreambuf_iterator<char>()));
}

/***********************************************************************************/
ModelPtr ResourceManager::GetModel(const std::string_view name, const std::string_view path) {

	// Check if model is already loaded.
	const auto val = m_modelCache.find(path.data());

	if (val == m_modelCache.end()) {
		// Load model, cache it, and return a shared_ptr to it.
		return m_modelCache.try_emplace(name.data(), std::make_shared<Model>(path, name, false, true)).first->second;
	}

	return val->second;
}

/***********************************************************************************/
ModelPtr ResourceManager::CacheModel(const std::string_view name, const Model model, const bool overwriteIfExists) {
	if (overwriteIfExists) {
		return m_modelCache.insert_or_assign(name.data(), std::make_shared<Model>(model)).first->second;
	}
	return m_modelCache.try_emplace(name.data(), std::make_shared<Model>(model)).first->second;
}

/***********************************************************************************/
std::optional<PBRMaterialPtr> ResourceManager::GetMaterial(const std::string_view name) const {

	// Check if material exists
	const auto val = m_materialCache.find(name.data());

	if (val == m_materialCache.end()) {
		return std::optional<PBRMaterialPtr>();
	}

	return std::make_optional<PBRMaterialPtr>(val->second);
}

/***********************************************************************************/
PBRMaterialPtr ResourceManager::CacheMaterial(const std::string_view name, const std::string_view albedoPath, const std::string_view aoPath, const std::string_view metallicPath, const std::string_view normalPath, const std::string_view roughnessPath, const std::string_view alphaMaskPath) {
	auto mat = PBRMaterial();
	mat.Init(name, albedoPath, aoPath, metallicPath, normalPath, roughnessPath, alphaMaskPath);

	return m_materialCache.try_emplace(name.data(), std::make_shared<PBRMaterial>(mat)).first->second;
}

/***********************************************************************************/
void ResourceManager::UnloadModel(const std::string_view modelName) {
	const auto model = m_modelCache.find(modelName.data());

	if (model != m_modelCache.end()) {
		model->second->Delete();

		m_modelCache.erase(modelName.data());
	}
}
