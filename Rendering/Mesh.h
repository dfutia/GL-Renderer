#ifndef MESH_H
#define MESH_H

#include "Graphics/VertexArray.h"
#include "Graphics/VertexBuffer.h"
#include "Animation/Animation.h"

struct Mesh
{
    std::shared_ptr<VertexArray> vao;
    std::shared_ptr<VertexBuffer> vbo;
    std::shared_ptr<VertexBuffer> ebo;
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    int VertexCount() const { return static_cast<int>(vertices.size()); }
    int IndexCount() const { return static_cast<int>(indices.size()); }
    const Vertex* Vertices() const { return vertices.empty() ? nullptr : &vertices[0]; }
};

struct SkinnedMesh
{
    std::shared_ptr<VertexArray> vao;
    std::shared_ptr<VertexBuffer> vbo;
    std::shared_ptr<VertexBuffer> ebo;
    std::vector<SkinnedVertex> vertices;
    std::vector<unsigned int> indices;
    Skeleton skeleton;

    int VertexCount() const { return static_cast<int>(vertices.size()); }
    int IndexCount() const { return static_cast<int>(indices.size()); }
};

Mesh CreateMesh(const float* vertices, size_t vertexCount, unsigned int stride);

#endif 