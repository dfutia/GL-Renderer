#ifndef SHADER_PROGRAM_H
#define SHADER_PROGRAM_H

#include <string>
#include <exception>

#include <glad/glad.h>
#include <glm/glm.hpp>

class Shader;

class LinkException : public std::exception
{
public:
	const char* what() const noexcept override
	{
		return "Shader program linking failed.";
	}
};

class ShaderProgram
{
public:
	typedef unsigned int Attribute;
	typedef unsigned int Uniform;

	ShaderProgram();
	ShaderProgram(const ShaderProgram& program);
	ShaderProgram(const Shader& vertex);
	ShaderProgram(const Shader& vertex, const Shader& fragment);
	ShaderProgram(const Shader& vertex, const Shader& fragment, const Shader& geometry);

	~ShaderProgram();

	operator unsigned int() const { return id; }
	const ShaderProgram& operator=(const ShaderProgram& other);

	void Attach(const Shader& shader);
	void TransformFeedbackVaryings(const char** varyings, unsigned int count);
	void Link();

	std::string GetInfoLog() const;

	Attribute GetAttribute(const std::string& name) const;
	Uniform GetUniform(const std::string& name) const;

	void SetUniform(const Uniform& uniform, int value) const;
	void SetUniform(const Uniform& uniform, float value) const;
	void SetUniform(const Uniform& uniform, const glm::vec2& value) const;
	void SetUniform(const Uniform& uniform, const glm::vec3& value) const;
	void SetUniform(const Uniform& uniform, const glm::vec4& value) const;
	void SetUniform(const Uniform& uniform, const float* values, unsigned int count) const;
	void SetUniform(const Uniform& uniform, const glm::vec2* values, unsigned int count) const;
	void SetUniform(const Uniform& uniform, const glm::vec3* values, unsigned int count) const;
	void SetUniform(const Uniform& uniform, const glm::vec4* values, unsigned int count) const;
	void SetUniform(const Uniform& uniform, const glm::mat3& value) const;
	void SetUniform(const Uniform& uniform, const glm::mat4& value) const;

	unsigned int GetUniformBlockIndex();
	void BindUniformBlock();
	//void BindUniformBlock();
	size_t GetUniformBlockSize();
	//size_t GetUniformBlockSize();
private:
	unsigned int id;
};

#endif 