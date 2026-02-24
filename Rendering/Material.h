#ifndef MATERIAL_H
#define MATERIAL_H

#include "Graphics/Texture.h"

struct MaterialProperties
{
	// phong/blinn-phong
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float shininess;

	// PBR
	glm::vec3 albedo;
	float metallic;
	float roughness;
	float ao;

	// extra
	glm::vec3 emission;
	float alpha;

	// texture transform
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
	Material(const Material& other) = default;
	~Material() = default;
	Material& operator=(const Material& other) = default;

	void SetTexture(const std::string& name, const Texture& texture);
	void RemoveTexture(const std::string& name);
	bool HasTexture(const std::string& name) const;
	const Texture& GetTexture(const std::string& name) const;

	const std::unordered_map<std::string, Texture>& GetAllTextures() const { return textures; }

	static Material CreateDefault();
	static Material CreateMetal();
	static Material CreateRough();
	static Material CreatePBRDefault();

	MaterialProperties properties;
	std::unordered_map<std::string, Texture> textures;
};

class MaterialLibrary
{

};

#endif