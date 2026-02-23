#include "BorderRenderer.h"
#include "Graphics/GraphicsDevice.h"
#include "Actors/Actor.h"
#include "Actors/TransformComponent.h"

void BorderRenderer::LoadBorders(const std::vector<CountryBorder>& countries)
{
    for (const auto& country : countries)
    {
        for (const auto& polygon : country.polygons)
        {
            if (polygon.size() < 2)
                continue;
            
            VertexDataBuffer buffer;
            for (const auto& point : polygon)
            {
                // Offset slightly above the sphere surface
                glm::vec3 elevated = point * (1.0f + heightOffset);
                buffer.Vec3(elevated);
            }
            
            BorderSegment segment;
            segment.vertexCount = polygon.size();
            segment.vbo = new VertexBuffer(buffer.Pointer(), buffer.Size(), VertexBuffer::StaticDraw);
            segment.vao = new VertexArray();
            segment.vao->BindAttribute(0, *segment.vbo, GL_FLOAT, 3, sizeof(float) * 3, 0);
            
            segments.push_back(segment);
        }
    }
    
    std::println("Created {} border segments", segments.size());
}

void BorderRenderer::Render(GraphicsDevice& graphics, const glm::mat4& view, const glm::mat4& proj)
{
    auto* transform = owner->GetComponent<TransformComponent>();
    if (!transform)
        return;
    
    glm::mat4 model = transform->GetMatrix();
    
    graphics.SetUniform("modelMatrix", model);
    graphics.SetUniform("viewMatrix", view);
    graphics.SetUniform("projectionMatrix", proj);
    graphics.SetUniform("color", glm::vec4(borderColor, 1.0f));
    
    glLineWidth(borderWidth);
    
    for (const auto& segment : segments)
    {
        graphics.BindResource(ResourceType::VERTEX_BUFFER, *segment.vao);
        glDrawArrays(GL_LINE_LOOP, 0, segment.vertexCount);
    }
    
    glLineWidth(1.0f);
}