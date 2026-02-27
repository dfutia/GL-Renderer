//// Rendering/LineRenderer.h
//#ifndef LINE_RENDERER_H
//#define LINE_RENDERER_H
//
//#include "Graphics/VertexArray.h"
//#include "Graphics/VertexBuffer.h"
//#include <vector>
//#include <glm/glm.hpp>
//
//struct LineStrip
//{
//    VertexArray* vao = nullptr;
//    VertexBuffer* vbo = nullptr;
//    int vertexCount = 0;
//};
//
//class LineRenderer
//{
//public:
//    LineRenderer() = default;
//    ~LineRenderer();
//
//    void AddLineStrip(const std::vector<glm::vec3>& points, bool loop = false);
//    void AddLine(const glm::vec3& a, const glm::vec3& b);
//    void Clear();
//
//    void Render(class GraphicsDevice& graphics, bool asLoops = true);
//
//    glm::vec3 color = glm::vec3(0.0f);
//    float lineWidth = 1.5f;
//
//private:
//    std::vector<LineStrip> strips;
//    std::vector<bool> isLoop;
//};
//
//#endif