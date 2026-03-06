#ifndef MODEL_H
#define MODEL_H

#include <utility>
#include <vector>

#include "Mesh.h"
#include "Material.h"
#include "Animation/Animation.h"

struct Model
{
    Mesh mesh;
    //Material material;
    //std::vector<Texture> textures;
};

std::expected<Model, std::string> LoadModel(const std::filesystem::path& filepath);
std::expected<Material, std::string> LoadMaterial(const std::filesystem::path& filepath);
std::expected<std::vector<Animation>, std::string> LoadAnimations(const std::filesystem::path& filepath);

#endif 