#ifndef MATERIAL_H
#define MATERIAL_H

#include "ShaderProgram.h"
#include "Texture.h"

class ShaderProgram;

typedef unsigned int Uniform;

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
	float ao; // ambient occlusion

	// extra
	glm::vec3 emission;
	float alpha;
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
		COUNT = 10
	};

	Material();
	Material(const Material& other);
	Material(const ShaderProgram& program);
	Material(const std::string& vertexShader, const std::string& fragmentShader);

	~Material() = default;

	const Material& operator=(const Material& other);

	void SetProgram(const ShaderProgram& program);
	const ShaderProgram& GetProgram() const;

	void SetTexture(TextureSlot slot, const Texture& texture);
	void SetTexture(TextureSlot slot, const std::string& filename);
	void RemoveTexture(TextureSlot slot);
	bool HasTexture(TextureSlot slot) const;
	const Texture& GetTexture(TextureSlot slot) const;

	void SetProperties(const MaterialProperties& properties);
	const MaterialProperties& GetProperties() const;
	MaterialProperties& GetProperties();

	void SetAmbient(const glm::vec3& ambient);
	void SetDiffuse(const glm::vec3& diffuse);
	void SetSpecular(const glm::vec3& specular);
	void SetShininess(float shininess);
	void SetAlbedo();
	void SetMetallic();
	void SetRoughness();
	void SetAO();
	void SetEmission();
	void SetAlpha();

	void SetUniform(const std::string& name, int value);
	void SetUniform(const std::string& name, float value);
	void SetUniform(const std::string& name, const glm::vec2& value);
	void SetUniform(const std::string& name, const glm::vec3& value);
	void SetUniform(const std::string& name, const glm::vec4& value);
	void SetUniform(const std::string& name, const glm::mat3& value);
	void SetUniform(const std::string& name, const glm::mat4& value);

	void Apply() const;

	static Material CreatePhongMaterial();
	static Material CreatePBRMaterial();
	static Material CreateUnlitMaterial();
private:
	ShaderProgram program;
	MaterialProperties properties;
	std::map<TextureSlot, Texture> textures;
	mutable std::map<std::string, Uniform> uniformCache;

	void ApplyTextures() const;
	void ApplyProperties() const;
	Uniform GetCachedUniform(const std::string& name) const;

	static std::string GetPhongVertexShader();
	static std::string GetPhongFragmentShader();
	static std::string GetPBRVertexShader();
	static std::string GetPBRFragmentShader();
	static std::string GetUnlitVertexShader();
	static std::string GetUnlitFragmentShader();
};

class MaterialLibrary
{

};

#endif