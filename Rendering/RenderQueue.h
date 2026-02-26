#ifndef RENDER_QUEUE_H
#define RENDER_QUEUE_H

#include <vector>
#include <algorithm>

#include "Material.h"
#include "Graphics/VertexArray.h"

namespace RenderQueue
{
    constexpr int Background = 0;
    constexpr int Opaque = 1000;
    constexpr int AlphaTest = 2000;
    constexpr int Transparent = 3000;
    constexpr int Overlay = 4000;
}

struct Renderable
{
    glm::mat4 modelMatrix;
    glm::mat3 normalMatrix;

    VertexArray* vao = nullptr;
    int indexCount = 0;
    int vertexCount = 0;

    Material* material = nullptr;
    std::string shader;
    std::string shadowShader;

    int queue = RenderQueue::Opaque;
    float depth = 0.0f;

    bool castsShadows = true;

    std::vector<glm::mat4>* boneMatrices = nullptr;
};

class RenderBatch
{
public:
    void Clear() { renderables.clear(); }

    void Add(const Renderable& r) { renderables.push_back(r); }

    void Sort()
    {
        std::sort(renderables.begin(), renderables.end(),
            [](const Renderable& a, const Renderable& b)
            {
                if (a.queue != b.queue)
                    return a.queue < b.queue;

                if (a.queue < RenderQueue::Transparent)
                    return a.depth < b.depth;
                else
                    return a.depth > b.depth;
            });
    }

    const std::vector<Renderable>& GetRenderables() const { return renderables; }

private:
    std::vector<Renderable> renderables;
};

#endif