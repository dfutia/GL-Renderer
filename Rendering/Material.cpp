#include "PCH.h"
#include "Material.h"

void Material::SetTexture(TextureSlot slot, const Texture& texture)
{
    textures[slot] = texture;
}

void Material::RemoveTexture(TextureSlot slot)
{
    textures.erase(slot);
}

bool Material::HasTexture(TextureSlot slot) const
{
    return textures.find(slot) != textures.end();
}

const Texture& Material::GetTexture(TextureSlot slot) const
{
    return textures.at(slot);
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