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

    void Load(const std::string& name,
        const std::filesystem::path& vertPath,
        const std::filesystem::path& fragPath,
        const std::vector<std::string>& defines)
    {
        std::string defineBlock = BuildDefineBlock(defines);

        std::string vertSource = ReadTextFile(vertPath);
        std::string fragSource = ReadTextFile(fragPath);

        // Insert defines after #version line
        vertSource = InjectDefines(vertSource, defineBlock);
        fragSource = InjectDefines(fragSource, defineBlock);

        ShaderProgram program(
            Shader(Shader::Vertex, vertSource),
            Shader(Shader::Fragment, fragSource)
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

    std::string BuildDefineBlock(const std::vector<std::string>& defines)
    {
        std::string block;
        for (const auto& def : defines)
        {
            block += "#define " + def + "\n";
        }
        return block;
    }

    std::string InjectDefines(const std::string& source, const std::string& defineBlock)
    {
        if (defineBlock.empty())
            return source;

        // Find end of #version line
        size_t versionPos = source.find("#version");
        if (versionPos == std::string::npos)
        {
            // No version, just prepend
            return defineBlock + source;
        }

        size_t lineEnd = source.find('\n', versionPos);
        if (lineEnd == std::string::npos)
            lineEnd = source.length();

        // Insert defines right after #version line
        std::string result = source.substr(0, lineEnd + 1);
        result += defineBlock;
        result += source.substr(lineEnd + 1);
        return result;
    }
};

#endif