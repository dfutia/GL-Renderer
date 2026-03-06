#ifndef MATERIAL_H
#define MATERIAL_H

#include <memory>
#include <string>
#include <unordered_map>

#include "Graphics/Texture.h"

struct MaterialProperties
{
    glm::vec3 ambient = glm::vec3(0.1f);
    glm::vec3 diffuse = glm::vec3(0.8f);
    glm::vec3 specular = glm::vec3(1.0f);
    float shininess = 32.0f;

    glm::vec3 albedo = glm::vec3(0.8f);
    float metallic = 0.0f;
    float roughness = 0.5f;
    float ao = 1.0f;

    glm::vec3 emission = glm::vec3(0.0f);
    float alpha = 1.0f;

    glm::vec2 tiling = glm::vec2(1.0f);
    glm::vec2 offset = glm::vec2(0.0f);
};

class Material
{
public:
    static constexpr const char* DIFFUSE = "diffuse";
    static constexpr const char* SPECULAR = "specular";
    static constexpr const char* NORMAL = "normal";
    static constexpr const char* HEIGHT = "heightMap";
    static constexpr const char* EMISSION = "emission";
    static constexpr const char* ALBEDO = "albedo";
    static constexpr const char* METALLIC = "metallic";
    static constexpr const char* ROUGHNESS = "roughness";
    static constexpr const char* AO = "ao";

    Material() = default;

    void SetTexture(const std::string& name, std::shared_ptr<Texture> texture);
    void RemoveTexture(const std::string& name);
    bool HasTexture(const std::string& name) const;
    const Texture& GetTexture(const std::string& name) const;
    const std::unordered_map<std::string, std::shared_ptr<Texture>>& GetAllTextures() const { return textures; }

    static Material CreateDefault();
    static Material CreateMetal();
    static Material CreateRough();
    static Material CreatePBRDefault();

    MaterialProperties properties;
private:
    std::unordered_map<std::string, std::shared_ptr<Texture>> textures;
};

#endif