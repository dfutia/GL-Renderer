#ifndef RENDER_DATA_H
#define RENDER_DATA_H

#include <glm/glm.hpp>

struct CameraUBO
{
    glm::mat4 view;
    glm::mat4 projection;
    glm::mat4 viewProjection;
    glm::vec4 position;
};

struct LightingUBO
{
    glm::vec4 direction;   
    glm::vec4 ambient;  
    glm::vec4 diffuse;
    glm::vec4 specular;
    glm::mat4 lightSpaceMatrix;
    glm::vec4 shadowParams;
};

#endif 