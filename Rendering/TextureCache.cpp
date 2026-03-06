#include "TextureCache.h"

TextureCache gTextureCache;


std::optional<std::reference_wrapper<Texture>> TextureCache::Get(const std::filesystem::path& path)
{
    auto it = textures.find(path.string());
    if (it != textures.end())
        return std::ref(it->second);
    return std::nullopt;
}

void TextureCache::Add(std::string key, Texture&& texture)
{
    textures.insert_or_assign(std::move(key), std::move(texture));
}