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
    Material material;
    std::vector<Texture> textures;
};

Model LoadModel(const std::string& filepath);
//SkinnedModel LoadSkinnedModel(const std::string& filepath);
std::vector<Animation> LoadAnimations(const std::string& filepath);

#endif 