#include "Mesh.h"

void ComputeTangents(std::vector<Vertex>& vertices)
{
    for (size_t i = 0; i < vertices.size(); i += 3)
    {
        Vertex& v0 = vertices[i];
        Vertex& v1 = vertices[i + 1];
        Vertex& v2 = vertices[i + 2];

        glm::vec3 edge1 = v1.position - v0.position;
        glm::vec3 edge2 = v2.position - v0.position;

        glm::vec2 deltaUV1 = v1.texCoords - v0.texCoords;
        glm::vec2 deltaUV2 = v2.texCoords - v0.texCoords;

        float denom = deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y;
        float f = (denom == 0.0f) ? 0.0f : 1.0f / denom;

        glm::vec3 tangent;
        tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
        tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
        tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
        tangent = glm::normalize(tangent);

        v0.tangent = tangent;
        v1.tangent = tangent;
        v2.tangent = tangent;
    }
}

Mesh CreateMesh(const float* vertices, size_t vertexCount, unsigned int stride)
{
    Mesh mesh;

    mesh.vertices.resize(vertexCount);
    for (size_t i = 0; i < vertexCount; i++)
    {
        const float* v = vertices + i * (stride / sizeof(float));
        mesh.vertices[i].position = glm::vec3(v[0], v[1], v[2]);
        mesh.vertices[i].normal = glm::vec3(v[3], v[4], v[5]);
        mesh.vertices[i].texCoords = glm::vec2(v[6], v[7]);
    }

    ComputeTangents(mesh.vertices);

    // Pack into buffer with tangents
    VertexDataBuffer buffer;
    for (const auto& v : mesh.vertices)
    {
        buffer.Vec3(v.position);
        buffer.Vec3(v.normal);
        buffer.Vec2(v.texCoords);
        buffer.Vec3(v.tangent);
    }

    unsigned int newStride = sizeof(float) * 11; // 3 + 3 + 2 + 3
    mesh.vbo = new VertexBuffer(buffer.Pointer(), buffer.Size(), VertexBuffer::StaticDraw);
    mesh.vao = new VertexArray();
    mesh.vao->BindAttribute(0, *mesh.vbo, GL_FLOAT, 3, newStride, 0);
    mesh.vao->BindAttribute(1, *mesh.vbo, GL_FLOAT, 3, newStride, sizeof(float) * 3);
    mesh.vao->BindAttribute(2, *mesh.vbo, GL_FLOAT, 2, newStride, sizeof(float) * 6);
    mesh.vao->BindAttribute(3, *mesh.vbo, GL_FLOAT, 3, newStride, sizeof(float) * 8);

    return mesh;
}