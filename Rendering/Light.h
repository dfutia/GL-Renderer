#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>

struct DirectionalLight
{
    glm::vec3 direction = glm::vec3(-0.2f, -1.0f, -0.3f);
    glm::vec3 color = glm::vec3(1.0f);
    float intensity = 1.0f;
};

struct PointLight
{

};

struct SpotLight
{

};

#endif 