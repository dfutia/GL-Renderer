#include "PCH.h"
#include "LineRenderer.h"
#include "Graphics/GraphicsDevice.h"

LineRenderer::~LineRenderer()
{
    Clear();
}

void LineRenderer::AddLineStrip(const std::vector<glm::vec3>& points, bool loop)
{
    if (points.size() < 2)
        return;

    VertexDataBuffer buffer;
    for (const auto& p : points)
        buffer.Vec3(p);

    LineStrip strip;
    strip.vertexCount = points.size();
    strip.vbo = new VertexBuffer(buffer.Pointer(), buffer.Size(), VertexBuffer::StaticDraw);
    strip.vao = new VertexArray();
    strip.vao->BindAttribute(0, *strip.vbo, GL_FLOAT, 3, sizeof(float) * 3, 0);

    strips.push_back(strip);
    isLoop.push_back(loop);
}

void LineRenderer::AddLine(const glm::vec3& a, const glm::vec3& b)
{
    AddLineStrip({ a, b }, false);
}

void LineRenderer::Clear()
{
    for (auto& strip : strips)
    {
        delete strip.vao;
        delete strip.vbo;
    }
    strips.clear();
    isLoop.clear();
}

void LineRenderer::Render(GraphicsDevice& graphics, bool asLoops)
{
    glLineWidth(lineWidth);
    graphics.SetUniform("color", glm::vec4(color, 1.0f));

    for (size_t i = 0; i < strips.size(); i++)
    {
        graphics.BindResource(ResourceType::VERTEX_BUFFER, *strips[i].vao);
        GLenum mode = isLoop[i] ? GL_LINE_LOOP : GL_LINE_STRIP;
        glDrawArrays(mode, 0, strips[i].vertexCount);
    }

    glLineWidth(1.0f);
}