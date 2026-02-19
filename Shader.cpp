#include "Shader.h"

Shader::Shader(const Shader& other)
{
	id = other.id;
}

Shader::Shader(ShaderType type)
{
	id = glCreateShader(type);
}

Shader::Shader(ShaderType type, const std::string& code)
{
	id = glCreateShader(type);
	Source(code);
	Compile();
}

//Shader::Shader(const ShaderSource& source)
//{
//	ShaderType glType = ConvertSourceType(source.GetType());
//	id = glCreateShader(glType);
//	Source(source.GetCode());
//	Compile();
//}

Shader::~Shader()
{
	glDeleteShader(id);
}

const Shader& Shader::operator=(const Shader& other)
{
	if (this != &other)
	{
		id = other.id;
	}
	return *this;
}

void Shader::Source(const std::string& code)
{
	const char* codeCStr = code.c_str();
	glShaderSource(id, 1, &codeCStr, nullptr);
}

void Shader::Compile()
{
	glCompileShader(id);

	int compileStatus;
	glGetShaderiv(id, GL_COMPILE_STATUS, &compileStatus);
	if (compileStatus == GL_FALSE)
	{
		throw CompileException();
	}
}

std::string Shader::GetInfoLog()
{
	int infoLogLength;
	glGetShaderiv(id, GL_INFO_LOG_LENGTH, &infoLogLength);
	std::string infoLog(infoLogLength, ' ');
	glGetShaderInfoLog(id, infoLogLength, nullptr, infoLog.data());
	return infoLog;
}

Shader::ShaderType Shader::ConvertSourceType(int sourceType)
{
	switch (sourceType)
	{
	case 0: // ShaderSource::Vertex
		return Shader::Vertex;
	case 1: // ShaderSource::Fragment
		return Shader::Fragment;
	case 2: // ShaderSource::Geometry
		return Shader::Geometry;
	default:
		throw CompileException(); // Unknown shader type
	}
}