#include "PCH.h"
#include "Material.h"

void Material::SetTexture(const std::string& name, std::shared_ptr<Texture> texture)
{
    textures.insert_or_assign(name, std::move(texture));
}

void Material::RemoveTexture(const std::string& name)
{
    textures.erase(name);
}

bool Material::HasTexture(const std::string& name) const
{
    return textures.find(name) != textures.end();
}

const Texture& Material::GetTexture(const std::string& name) const
{
    return *textures.at(name);
}

Material Material::CreateDefault()
{
    Material material;
    material.properties.ambient = glm::vec3(0.1f);
    material.properties.diffuse = glm::vec3(0.8f);
    material.properties.specular = glm::vec3(1.0f);
    material.properties.shininess = 32.0f;
    return material;
}

Material Material::CreateMetal()
{
    Material material;
    material.properties.ambient = glm::vec3(0.05f);
    material.properties.diffuse = glm::vec3(0.4f);
    material.properties.specular = glm::vec3(1.0f);
    material.properties.shininess = 128.0f;
    return material;
}

Material Material::CreateRough()
{
    Material material;
    material.properties.ambient = glm::vec3(0.1f);
    material.properties.diffuse = glm::vec3(0.6f);
    material.properties.specular = glm::vec3(0.2f);
    material.properties.shininess = 8.0f;
    return material;
}

Material Material::CreatePBRDefault()
{
    Material material;
    material.properties.albedo = glm::vec3(0.8f);
    material.properties.metallic = 0.0f;
    material.properties.roughness = 0.5f;
    material.properties.ao = 1.0f;
    return material;
}