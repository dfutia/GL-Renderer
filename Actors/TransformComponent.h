#ifndef TRANSFORM_COMPONENT_H
#define TRANSFORM_COMPONENT_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "ActorComponent.h"

class TransformComponent : public ActorComponent
{
public:
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 rotation = glm::vec3(0.0f);
    glm::vec3 scale = glm::vec3(1.0f);

    const char* GetName() const override { return "Transform"; }

    void RegisterProperties(PropertyRegistry& registry) override
    {
        registry.Vec3("Position", &position, -10000.0f, 10000.0f, 0.1f);
        registry.Vec3("Rotation", &rotation, -360.0f, 360.0f, 0.5f);
        registry.Vec3("Scale", &scale, 0.001f, 1000.0f, 0.01f);
    }

    glm::mat4 GetMatrix() const
    {
        glm::mat4 m = glm::mat4(1.0f);
        m = glm::translate(m, position);
        m = glm::rotate(m, glm::radians(rotation.x), glm::vec3(1, 0, 0));
        m = glm::rotate(m, glm::radians(rotation.y), glm::vec3(0, 1, 0));
        m = glm::rotate(m, glm::radians(rotation.z), glm::vec3(0, 0, 1));
        m = glm::scale(m, scale);
        return m;
    }

    glm::mat3 GetNormalMatrix() const
    {
        return glm::transpose(glm::inverse(glm::mat3(GetMatrix())));
    }
};

#endif 