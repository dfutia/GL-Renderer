#ifndef MESH_H
#define MESH_H

#include <sstream>

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

    std::string DebugInfo(bool showVertices = false) const
    {
        std::ostringstream ss;
        ss << "Mesh: " << vertices.size() << " vertices, " << indices.size() << " indices\n";
        ss << "  VAO: " << (vao ? "valid" : "null") << "\n";
        ss << "  VBO: " << (vbo ? "valid" : "null") << "\n";
        ss << "  EBO: " << (ebo ? "valid" : "null") << "\n";
        if (showVertices)
        {
            for (size_t i = 0; i < vertices.size(); i++)
                ss << "  [" << i << "] " << vertices[i].DebugInfo() << "\n";
        }
        return ss.str();
    }
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

    std::string DebugInfo(bool showVertices = false, bool showSkeleton = true) const
    {
        std::ostringstream ss;
        ss << "SkinnedMesh: " << vertices.size() << " vertices, " << indices.size() << " indices\n";
        ss << "  VAO: " << (vao ? "valid" : "null") << "\n";
        ss << "  VBO: " << (vbo ? "valid" : "null") << "\n";
        ss << "  EBO: " << (ebo ? "valid" : "null") << "\n";
        if (showSkeleton)
            ss << skeleton.DebugInfo();
        if (showVertices)
        {
            for (size_t i = 0; i < vertices.size(); i++)
                ss << "  [" << i << "] " << vertices[i].DebugInfo() << "\n";
        }
        return ss.str();
    }
};

Mesh CreateMesh(const float* vertices, size_t vertexCount, unsigned int stride);

#endif 