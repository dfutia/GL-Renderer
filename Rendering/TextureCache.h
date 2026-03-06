#ifndef TEXTURE_CACHE_H
#define TEXTURE_CACHE_H

#include <unordered_map>
#include <string>
#include <optional>
#include <filesystem>

#include "Graphics/Texture.h"

class TextureCache
{
public:
	std::optional<std::reference_wrapper<Texture>> Get(const std::filesystem::path& path);
	void Add(std::string key, Texture&& texture);
private:
	std::unordered_map<std::string, Texture> textures;
};

extern TextureCache gTextureCache;

#endif 