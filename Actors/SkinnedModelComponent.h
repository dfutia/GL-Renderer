#ifndef SKINNED_MODEL_COMPONENT_H
#define SKINNED_MODEL_COMPONENT_H

#include "ActorComponent.h"
#include "Rendering/Mesh.h"
#include "Rendering/Material.h"
#include "Animation/Animation.h"

class SkinnedModelComponent : public ActorComponent
{
public:
    SkinnedModelComponent() = default;

    void OnUpdate(float deltaTime) override
    {
        if (animator)
            animator->Update(deltaTime);
    }

    const std::vector<glm::mat4>& GetBoneMatrices() const
    {
        return animator->GetFinalBoneMatrices();
    }

    void SetModel(const Mesh& m)
    {
        mesh = m;
        animator.emplace(*mesh.skeleton);
    }

    void SetMaterial(const Material& mat) { material = mat; }

    Mesh mesh;
    Material material;
    std::optional<Animator> animator;
};

#endif 