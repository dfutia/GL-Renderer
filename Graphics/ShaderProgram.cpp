#include "ShaderProgram.h"
#include "Shader.h"

ShaderProgram::ShaderProgram()
{
	id = glCreateProgram();
}

ShaderProgram::ShaderProgram(const ShaderProgram& program)
{
	id = program.id;
}

ShaderProgram::ShaderProgram(const Shader& vertex)
{
	id = glCreateProgram();
	Attach(vertex);
	Link();
}

ShaderProgram::ShaderProgram(const Shader& vertex, const Shader& fragment)
{
	id = glCreateProgram();
	Attach(vertex);
	Attach(fragment);
	Link();
}

ShaderProgram::ShaderProgram(const Shader& vertex, const Shader& fragment, const Shader& geometry)
{
	id = glCreateProgram();
	Attach(vertex);
	Attach(fragment);
	Attach(geometry);
	Link();
}

ShaderProgram::~ShaderProgram()
{
	//glDeleteProgram(id);
}

const ShaderProgram& ShaderProgram::operator=(const ShaderProgram& other)
{
	if (this != &other)
	{
		id = other.id;
	}
	return *this;
}

void ShaderProgram::Attach(const Shader& shader)
{
	glAttachShader(id, shader);
}

void ShaderProgram::TransformFeedbackVaryings(const char** varyings, unsigned int count)
{
	glTransformFeedbackVaryings(id, count, varyings, GL_INTERLEAVED_ATTRIBS);
}

void ShaderProgram::Link()
{
	glLinkProgram(id);

	int linkStatus;
	glGetProgramiv(id, GL_LINK_STATUS, &linkStatus);
	if (linkStatus == GL_FALSE)
	{
		throw LinkException();
	}
}

std::string ShaderProgram::GetInfoLog() const
{
	int infoLogLength;
	glGetProgramiv(id, GL_INFO_LOG_LENGTH, &infoLogLength);
	std::string infoLog(infoLogLength, ' ');
	glGetProgramInfoLog(id, infoLogLength, nullptr, infoLog.data());
	return infoLog;
}

ShaderProgram::Attribute ShaderProgram::GetAttribute(const std::string& name) const
{
	return glGetAttribLocation(id, name.c_str());
}

ShaderProgram::Uniform ShaderProgram::GetUniform(const std::string& name) const
{
	return glGetUniformLocation(id, name.c_str());
}

void ShaderProgram::SetUniform(const Uniform& uniform, int value) const
{
	glUniform1i(uniform, value);
}

void ShaderProgram::SetUniform(const Uniform& uniform, float value) const
{
	glUniform1f(uniform, value);
}

void ShaderProgram::SetUniform(const Uniform& uniform, const glm::vec2& value) const
{
	glUniform2fv(uniform, 1, &value[0]);
}

void ShaderProgram::SetUniform(const Uniform& uniform, const glm::vec3& value) const
{
	glUniform3fv(uniform, 1, &value[0]);
}

void ShaderProgram::SetUniform(const Uniform& uniform, const glm::vec4& value) const
{
	glUniform4fv(uniform, 1, &value[0]);
}

void ShaderProgram::SetUniform(const Uniform& uniform, const float* values, unsigned int count) const
{
	glUniform1fv(uniform, count, values);
}

void ShaderProgram::SetUniform(const Uniform& uniform, const glm::vec2* values, unsigned int count) const
{
	glUniform2fv(uniform, count, &values[0][0]);
}

void ShaderProgram::SetUniform(const Uniform& uniform, const glm::vec3* values, unsigned int count) const
{
	glUniform3fv(uniform, count, &values[0][0]);
}

void ShaderProgram::SetUniform(const Uniform& uniform, const glm::vec4* values, unsigned int count) const
{
	glUniform4fv(uniform, count, &values[0][0]);
}

void ShaderProgram::SetUniform(const Uniform& uniform, const glm::mat3& value) const
{
	glUniformMatrix3fv(uniform, 1, GL_FALSE, &value[0][0]);
}

void ShaderProgram::SetUniform(const Uniform& uniform, const glm::mat4& value) const
{
	glUniformMatrix4fv(uniform, 1, GL_FALSE, &value[0][0]);
}
