#ifndef RENDER_COMPONENT_H
#define RENDER_COMPONENT_H

#include <string>
#include <unordered_map>
#include <variant>

#include "ActorComponent.h"
#include "Rendering/RenderQueue.h"

class GraphicsDevice;

class RenderComponent : public ActorComponent
{
public:
    std::string shader = "phong";
    std::string shadowShader = "depth";

    int renderQueue = RenderQueue::Opaque;
    bool castsShadows = true;
    bool visible = true;

    // For sorting within the same queue
    // Opaque: lower depth first (front-to-back)
    // Transparent: higher depth first (back-to-front)
    float sortingOrder = 0.0f;

    // Separate maps for each type
    std::unordered_map<std::string, int> uniformsInt;
    std::unordered_map<std::string, float> uniformsFloat;
    std::unordered_map<std::string, glm::vec2> uniformsVec2;
    std::unordered_map<std::string, glm::vec3> uniformsVec3;
    std::unordered_map<std::string, glm::vec4> uniformsVec4;
    std::unordered_map<std::string, glm::mat3> uniformsMat3;
    std::unordered_map<std::string, glm::mat4> uniformsMat4;

    void SetUniform(const std::string& name, int value) { uniformsInt[name] = value; }
    void SetUniform(const std::string& name, float value) { uniformsFloat[name] = value; }
    void SetUniform(const std::string& name, const glm::vec2& value) { uniformsVec2[name] = value; }
    void SetUniform(const std::string& name, const glm::vec3& value) { uniformsVec3[name] = value; }
    void SetUniform(const std::string& name, const glm::vec4& value) { uniformsVec4[name] = value; }
    void SetUniform(const std::string& name, const glm::mat3& value) { uniformsMat3[name] = value; }
    void SetUniform(const std::string& name, const glm::mat4& value) { uniformsMat4[name] = value; }

    void ApplyUniforms(ShaderProgram& shader) const;

    // Inspector support
    const char* GetName() const override { return "Render"; }

    void RegisterProperties(PropertyRegistry& registry) override
    {
        registry.String("Shader", &shader);
        registry.String("Shadow Shader", &shadowShader);
        registry.Int("Render Queue", &renderQueue, 0, 10000);
        registry.Float("Sorting Order", &sortingOrder, -1000.0f, 1000.0f, 0.1f);
        registry.Bool("Casts Shadows", &castsShadows);
        registry.Bool("Visible", &visible);
    }
};

#endif