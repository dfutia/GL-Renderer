#ifndef MODEL_COMPONENT_H
#define MODEL_COMPONENT_H

#include "ActorComponent.h"
#include "Rendering/Mesh.h"
#include "Rendering/Material.h"
#include "Graphics/ShaderProgram.h"

class ModelComponent : public ActorComponent
{
public:
    ModelComponent() = default;
    ModelComponent(const Mesh& mesh, const Material& material)
        : mesh(mesh), material(material) {
    }

    const char* GetName() const override { return "Model"; }

    void RegisterProperties(PropertyRegistry& registry) override
    {
        registry.Color3("Ambient", &material.properties.ambient);
        registry.Color3("Diffuse", &material.properties.diffuse);
        registry.Color3("Specular", &material.properties.specular);
        registry.Float("Shininess", &material.properties.shininess, 1.0f, 256.0f, 1.0f);
    }

    Mesh mesh;
    Material material;
    ShaderProgram* shader = nullptr;
};


#endif 