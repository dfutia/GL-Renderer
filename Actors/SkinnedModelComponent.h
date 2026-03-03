#ifndef SKINNED_MODEL_COMPONENT_H
#define SKINNED_MODEL_COMPONENT_H

#include "ActorComponent.h"
#include "Rendering/Mesh.h"
#include "Rendering/Material.h"
#include "Animation/Animation.h"

class SkinnedModelComponent : public ActorComponent
{
public:
    SkinnedModelComponent(const Mesh& mesh, const Material& material) : mesh(mesh), material(material), animator(*mesh.skeleton) {
    }

    void OnUpdate(float deltaTime) override
    {
        animator.Update(deltaTime);
    }

    const std::vector<glm::mat4>& GetBoneMatrices() const
    {
        return animator.GetFinalBoneMatrices();
    }

    Mesh mesh;
    Material material;
    Animator animator;
};

#endif 