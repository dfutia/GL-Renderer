#ifndef SHADER_LIBRARY_H
#define SHADER_LIBRARY_H

#include "Graphics/ShaderProgram.h"
#include "Graphics/Shader.h"
#include "Platform/File.h"

class ShaderLibrary
{
public:
    void Load(const std::string& name,
        const std::filesystem::path& vertPath,
        const std::filesystem::path& fragPath)
    {
        ShaderProgram program(
            Shader(Shader::Vertex, ReadTextFile(vertPath)),
            Shader(Shader::Fragment, ReadTextFile(fragPath))
        );
        shaders[name] = program;
    }

    ShaderProgram* Get(const std::string& name)
    {
        auto it = shaders.find(name);
        return it != shaders.end() ? &it->second : nullptr;
    }

    bool Has(const std::string& name) const
    {
        return shaders.find(name) != shaders.end();
    }

private:
    std::unordered_map<std::string, ShaderProgram> shaders;
};

#endif