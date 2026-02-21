#include "Mesh.h"

Mesh CreateMesh(const float* vertices, size_t vertexCount, unsigned int stride)
{
    Mesh mesh;

    // Store vertices if you need CPU-side access later
    mesh.vertices.resize(vertexCount);
    for (size_t i = 0; i < vertexCount; i++)
    {
        const float* v = vertices + i * (stride / sizeof(float));
        mesh.vertices[i].position = glm::vec3(v[0], v[1], v[2]);
        mesh.vertices[i].normal = glm::vec3(v[3], v[4], v[5]);
        mesh.vertices[i].texCoords = glm::vec2(v[6], v[7]);
    }

    mesh.vbo = new VertexBuffer(vertices, vertexCount * stride, VertexBuffer::StaticDraw);
    mesh.vao = new VertexArray();
    mesh.vao->BindAttribute(0, *mesh.vbo, GL_FLOAT, 3, stride, 0);
    mesh.vao->BindAttribute(1, *mesh.vbo, GL_FLOAT, 3, stride, sizeof(float) * 3);
    mesh.vao->BindAttribute(2, *mesh.vbo, GL_FLOAT, 2, stride, sizeof(float) * 6);

    return mesh;
}