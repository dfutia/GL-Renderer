#ifndef MODEL_H
#define MODEL_H

#include "Mesh.h"
#include "Material.h"
#include "Animation/Animation.h"

struct Model
{
    Mesh mesh;
    Material material;

    std::string DebugInfo() const
    {
        std::ostringstream ss;
        ss << "=== Model ===\n";
        ss << mesh.DebugInfo();
        ss << material.DebugInfo();
        return ss.str();
    }
};

struct SkinnedModel
{
    SkinnedMesh mesh;
    Material material;

    std::string DebugInfo() const
    {
        std::ostringstream ss;
        ss << "=== SkinnedModel ===\n";
        ss << mesh.DebugInfo();
        ss << material.DebugInfo();
        return ss.str();
    }
};

Model LoadModel(const std::string& filepath);
SkinnedModel LoadSkinnedModel(const std::string& filepath);
std::vector<Animation> LoadAnimations(const std::string& filepath);

#endif 