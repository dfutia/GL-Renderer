#ifndef BORDER_RENDERER_H
#define BORDER_RENDERER_H

#include "Actors/ActorComponent.h"
#include "Graphics/VertexArray.h"
#include "Graphics/VertexBuffer.h"
#include "GeoData.h"

struct BorderSegment
{
    VertexArray* vao;
    VertexBuffer* vbo;
    int vertexCount;
};

class BorderRenderer : public ActorComponent
{
public:
    glm::vec3 borderColor = glm::vec3(0.0f);  // Black
    float borderWidth = 1.5f;
    float heightOffset = 0.002f;  // Slight offset to render above terrain

    void LoadBorders(const std::vector<CountryBorder>& countries);
    void Render(class GraphicsDevice& graphics, const glm::mat4& view, const glm::mat4& proj);

    const char* GetName() const override { return "Border Renderer"; }

    void RegisterProperties(PropertyRegistry& registry) override
    {
        registry.Color3("Border Color", &borderColor);
        registry.Float("Border Width", &borderWidth, 0.5f, 5.0f, 0.1f);
        registry.Float("Height Offset", &heightOffset, 0.0f, 0.05f, 0.001f);
    }

private:
    std::vector<BorderSegment> segments;
};

#endif
