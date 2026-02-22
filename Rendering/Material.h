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
	enum TextureSlot
	{
		Diffuse = 0,
		Specular = 1,
		Normal = 2,
		Height = 3,
		Emission = 4,
		Albedo = 5,
		Metallic = 6,
		Roughness = 7,
		AO = 8,
		Combined_MetallicRoughnessAO = 9, // R=AO, G=Roughness, B=Metallic
		Shadow = 10,
		COUNT = 11
	};

	Material() = default;
	Material(const Material& other) = default;
	~Material() = default;
	Material& operator=(const Material& other) = default;

	void SetTexture(TextureSlot slot, const Texture& texture);
	void SetTexture(TextureSlot slot, const std::string& filename);
	void RemoveTexture(TextureSlot slot);
	bool HasTexture(TextureSlot slot) const;
	const Texture& GetTexture(TextureSlot slot) const;

	static Material CreateDefault();
	static Material CreateMetal();
	static Material CreateRough();
	static Material CreatePBRDefault();

	MaterialProperties properties;
	std::map<TextureSlot, Texture> textures;
};

class MaterialLibrary
{

};

#endif