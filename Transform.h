#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct Transform
{
    glm::vec3 position{ 0.0f };
    glm::vec3 rotation{ 0.0f };  // Euler angles in degrees
    glm::vec3 scale{ 1.0f };

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
};

#endif