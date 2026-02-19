#ifndef MESH_H
#define MESH_H

#include "VertexArray.h"
#include "VertexBuffer.h"
#include "Animation.h"

struct Mesh
{
    VertexArray* vao;
    VertexBuffer* vbo;
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    int VertexCount() const
    {
        return vertices.size();
    }

    int IndexCount() const
    {
        return indices.size();
    }

    const Vertex* Vertices() const
    {
        return &vertices[0];
    }
};

struct SkinnedMesh
{
    VertexArray* vao = nullptr;
    VertexBuffer* vbo = nullptr;
    std::vector<SkinnedVertex> vertices;
    std::vector<unsigned int> indices;
    Skeleton skeleton;
    std::vector<Animation> animations;

    int VertexCount() const { return static_cast<int>(vertices.size()); }
    int IndexCount() const { return static_cast<int>(indices.size()); }
};

#endif 