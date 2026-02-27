#include "RenderComponent.h"
#include "Graphics/ShaderProgram.h"

void RenderComponent::ApplyUniforms(ShaderProgram& shader) const
{
    for (const auto& [name, value] : uniformsInt)
        shader.SetUniform(name, value);

    for (const auto& [name, value] : uniformsFloat)
        shader.SetUniform(name, value);

    for (const auto& [name, value] : uniformsVec2)
        shader.SetUniform(name, value);

    for (const auto& [name, value] : uniformsVec3)
        shader.SetUniform(name, value);

    for (const auto& [name, value] : uniformsVec4)
        shader.SetUniform(name, value);

    for (const auto& [name, value] : uniformsMat3)
        shader.SetUniform(name, value);

    for (const auto& [name, value] : uniformsMat4)
        shader.SetUniform(name, value);
}