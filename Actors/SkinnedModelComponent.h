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
    SkinnedModelComponent(const SkinnedMesh& mesh, const Material& material)
        : mesh(mesh), material(material), animator(mesh.skeleton) {
    }

    void PlayAnimation(int index)
    {
        if (index >= 0 && index < mesh.animations.size())
            animator.Play(&mesh.animations[index]);
    }

    void Update(float deltaTime)
    {
        animator.Update(deltaTime);
    }

    const std::vector<glm::mat4>& GetBoneMatrices() const
    {
        return animator.GetFinalBoneMatrices();
    }

    SkinnedMesh mesh;
    Material material;
    Animator animator;
};

#endif 