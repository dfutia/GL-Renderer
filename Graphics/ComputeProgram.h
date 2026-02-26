#ifndef COMPUTE_PROGRAM_H
#define COMPUTE_PROGRAM_H

#include "Graphics/Shader.h"
#include "Graphics/ShaderProgram.h"

class ComputeProgram
{
public:
    ComputeProgram() : id(0) {}

    ComputeProgram(const std::string& source)
    {
        id = glCreateProgram();
        Shader computeShader(Shader::Compute, source);
        glAttachShader(id, computeShader);
        glLinkProgram(id);

        int linkStatus;
        glGetProgramiv(id, GL_LINK_STATUS, &linkStatus);
        if (linkStatus == GL_FALSE)
        {
            char log[512];
            glGetProgramInfoLog(id, 512, nullptr, log);
            throw std::runtime_error(std::string("Compute shader link failed: ") + log);
        }
    }

    void Use() const { glUseProgram(id); }

    void Dispatch(unsigned int groupsX, unsigned int groupsY, unsigned int groupsZ)
    {
        glDispatchCompute(groupsX, groupsY, groupsZ);
    }

    void MemoryBarrier(GLbitfield barriers = GL_SHADER_STORAGE_BARRIER_BIT)
    {
        glMemoryBarrier(barriers);
    }

    // Uniform setters (reuse from ShaderProgram or add here)
    void SetUniform(const std::string& name, int value)
    {
        glUniform1i(glGetUniformLocation(id, name.c_str()), value);
    }

    void SetUniform(const std::string& name, float value)
    {
        glUniform1f(glGetUniformLocation(id, name.c_str()), value);
    }

    void SetUniform(const std::string& name, const glm::vec3& value)
    {
        glUniform3fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
    }

    unsigned int GetID() const { return id; }

private:
    unsigned int id;
};

#endif