#ifndef MODEL_COMPONENT_H
#define MODEL_COMPONENT_H

#include "ActorComponent.h"
#include "Rendering/Mesh.h"
#include "Rendering/Material.h"

class ModelComponent : public ActorComponent
{
public:
    ModelComponent() = default;
    ModelComponent(const Mesh& mesh, const Material& material)
        : mesh(mesh), material(material) {
    }

    Mesh mesh;
    Material material;
};


#endif 