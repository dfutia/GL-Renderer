#include "PCH.h"
#include "Material.h"
#include "ShaderProgram.h"
#include "Shader.h"
#include "File.h"

Material::Material()
{
	*this = CreateUnlitMaterial();
}

Material::Material(const Material& other) :
	program(other.program),
	properties(other.properties),
	textures(other.textures),
	uniformCache(other.uniformCache) {}

Material::Material(const ShaderProgram& program) : program(program) {}

Material::Material(const std::string& vertexShader, const std::string& fragmentShader)
{
	Shader vertex(Shader::ShaderType::Vertex, vertexShader);
	Shader fragment(Shader::ShaderType::Fragment, fragmentShader);
	program = ShaderProgram(vertex, fragment);
}

const Material& Material::operator=(const Material& other)
{
	if (this != &other)
	{
		program = other.program;
		properties = other.properties;
		textures = other.textures;
		uniformCache = other.uniformCache;
	}
	return *this;
}

void Material::SetProgram(const ShaderProgram& program)
{
	this->program = program;
	uniformCache.clear();
}

const ShaderProgram& Material::GetProgram() const
{
	return program;
}

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

void Material::SetProperties(const MaterialProperties& properties)
{
	this->properties = properties;
}

const MaterialProperties& Material::GetProperties() const
{
	return properties;
}

MaterialProperties& Material::GetProperties()
{
	return properties;
}

void Material::SetAmbient(const glm::vec3& ambient)
{
	properties.ambient = ambient;
}

void Material::SetDiffuse(const glm::vec3& diffuse)
{
	properties.diffuse = diffuse;
}

void Material::SetSpecular(const glm::vec3& specular)
{
	properties.specular = specular;
}

void Material::SetShininess(float shininess)
{
	properties.shininess = shininess;
}

void Material::SetEmission()
{
}

void Material::SetAlpha()
{
}

void Material::SetUniform(const std::string& name, int value)
{
	Uniform uniform = GetCachedUniform(name);
	program.SetUniform(uniform, value);
}

void Material::SetUniform(const std::string& name, float value)
{
	Uniform uniform = GetCachedUniform(name);
	program.SetUniform(uniform, value);
}

void Material::SetUniform(const std::string& name, const glm::vec2& value)
{
	Uniform uniform = GetCachedUniform(name);
	program.SetUniform(uniform, value);
}

void Material::SetUniform(const std::string& name, const glm::vec3& value)
{
	Uniform uniform = GetCachedUniform(name);
	program.SetUniform(uniform, value);
}

void Material::SetUniform(const std::string& name, const glm::vec4& value)
{
	Uniform uniform = GetCachedUniform(name);
	program.SetUniform(uniform, value);
}

void Material::SetUniform(const std::string& name, const glm::mat3& value)
{
	Uniform uniform = GetCachedUniform(name);
	program.SetUniform(uniform, value);
}

void Material::SetUniform(const std::string& name, const glm::mat4& value)
{
	Uniform uniform = GetCachedUniform(name);
	program.SetUniform(uniform, value);
}

void Material::Apply() const
{
	glUseProgram(program);
	ApplyTextures();
	ApplyProperties();
}

Uniform Material::GetCachedUniform(const std::string& name) const
{
	auto it = uniformCache.find(name);
	if (it != uniformCache.end())
	{
		return it->second;
	}
	else
	{
		Uniform uniform = program.GetUniform(name);
		uniformCache[name] = uniform;
		return uniform;
	}
}

void Material::ApplyTextures() const
{
	int textureUnit = 0;
	for (const auto& pair : textures)
	{
		glActiveTexture(GL_TEXTURE0 + textureUnit);
		glBindTexture(GL_TEXTURE_2D, pair.second);
		Uniform uniform = program.GetUniform("texture" + std::to_string(pair.first));
		program.SetUniform(uniform, textureUnit);
		textureUnit++;
	}
}

void Material::ApplyProperties() const
{
	Uniform ambientUniform = GetCachedUniform("material.ambient");
	program.SetUniform(ambientUniform, properties.ambient);
	Uniform diffuseUniform = GetCachedUniform("material.diffuse");
	program.SetUniform(diffuseUniform, properties.diffuse);
	Uniform specularUniform = GetCachedUniform("material.specular");
	program.SetUniform(specularUniform, properties.specular);
	Uniform shininessUniform = GetCachedUniform("material.shininess");
	program.SetUniform(shininessUniform, properties.shininess);
	Uniform alphaUniform = GetCachedUniform("material.alpha");
	program.SetUniform(alphaUniform, properties.alpha);
}

Material Material::CreatePhongMaterial()
{
	Material material(GetPhongVertexShader(), GetPhongFragmentShader());

	MaterialProperties props;
	props.ambient = glm::vec3(0.1f, 0.1f, 0.1f);
	props.diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
	props.specular = glm::vec3(1.0f, 1.0f, 1.0f);
	props.shininess = 32.0f;
	material.SetProperties(props);

	return material;
}

Material Material::CreatePBRMaterial()
{
	Material material(GetPBRVertexShader(), GetPBRFragmentShader());

	MaterialProperties props;
	props.albedo = glm::vec3(0.8f, 0.8f, 0.8f);
	props.metallic = 0.0f;
	props.roughness = 0.5f;
	props.ao = 1.0f;
	material.SetProperties(props);

	return material;
}

Material Material::CreateUnlitMaterial()
{
	Material material(GetUnlitVertexShader(), GetUnlitFragmentShader());

	MaterialProperties props;
	props.diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
	props.alpha = 1.0f;
	material.SetProperties(props);

	return material;
}

std::string Material::GetPhongVertexShader()
{
	return ReadTextFile((GetMediaPath() / "Shaders/phong.vert"));
}

std::string Material::GetPhongFragmentShader()
{
	return ReadTextFile((GetMediaPath() / "Shaders/phong.frag"));
}

std::string Material::GetPBRVertexShader()
{
	return ReadTextFile((GetMediaPath() / "Shaders/pbr.vert"));
}

std::string Material::GetPBRFragmentShader()
{
	return ReadTextFile((GetMediaPath() / "Shaders/pbr.frag"));
}

std::string Material::GetUnlitVertexShader()
{
	return ReadTextFile((GetMediaPath() / "Shaders/unlit.vert"));
}

std::string Material::GetUnlitFragmentShader()
{
	return ReadTextFile((GetMediaPath() / "Shaders/unlit.frag"));
}