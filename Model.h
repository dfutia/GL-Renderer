#ifndef MODEL_H
#define MODEL_H

#include "Mesh.h"
#include "Material.h"

struct Model
{
    Mesh mesh;
    Material material;
};

struct SkinnedModel
{
    SkinnedMesh mesh;
    Material material;
};

Model LoadModel(const std::string& filepath);
SkinnedModel LoadSkinnedModel(const std::string& filepath);
std::vector<Animation> LoadAnimations(const std::string& filepath);

#endif 