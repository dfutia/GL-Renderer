#include "RenderComponent.h"
#include "Graphics/GraphicsDevice.h"

void RenderComponent::ApplyUniforms(GraphicsDevice& graphics) const
{
    for (const auto& [name, value] : uniformsInt)
        graphics.SetUniform(name, value);

    for (const auto& [name, value] : uniformsFloat)
        graphics.SetUniform(name, value);

    for (const auto& [name, value] : uniformsVec2)
        graphics.SetUniform(name, value);

    for (const auto& [name, value] : uniformsVec3)
        graphics.SetUniform(name, value);

    for (const auto& [name, value] : uniformsVec4)
        graphics.SetUniform(name, value);

    for (const auto& [name, value] : uniformsMat3)
        graphics.SetUniform(name, value);

    for (const auto& [name, value] : uniformsMat4)
        graphics.SetUniform(name, value);
}