#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <exception>

#include <glad/glad.h>

#define GLSL( x ) "#version 330 core\n" #x

class ShaderSource;

class CompileException : public std::exception
{
public:
	const char* what() const noexcept override
	{
		return "Shader compilation failed.";
	}
};

class Shader
{
public:
	enum ShaderType
	{
		Vertex = GL_VERTEX_SHADER,
		Fragment = GL_FRAGMENT_SHADER,
		Geometry = GL_GEOMETRY_SHADER,
		Compute = GL_COMPUTE_SHADER
	};

	Shader(const Shader& other);
	Shader(ShaderType type);
	Shader(ShaderType type, const std::string& code);
	//Shader(const ShaderSource& source);

	~Shader();

	operator unsigned int() const { return id; }
	const Shader& operator=(const Shader& other);

	void Source(const std::string& code);
	void Compile();

	std::string GetInfoLog();

	static ShaderType ConvertSourceType(int sourceType);
private:
	unsigned int id;
};

#endif