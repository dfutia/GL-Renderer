#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>

struct DirectionalLight
{
    glm::vec3 direction = glm::vec3(-0.2f, -1.0f, -0.3f);
    glm::vec3 color = glm::vec3(1.0f);
    float intensity = 1.0f;
    float shadowBias = 0.005f;

    const Texture* shadowMap = nullptr;
    glm::mat4 lightSpaceMatrix = glm::mat4(1.0f);

    unsigned int shadowWidth = 4096;
    unsigned int shadowHeight = 4096;
    float shadowNearPlane = 1.0f;
    float shadowFarPlane = 1500.0f;
    float shadowOrthoSize = 600.0f;
    float shadowDistance = 500.0f;
};

struct PointLight
{

};

struct SpotLight
{

};

#endif 