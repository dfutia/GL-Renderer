#include "PCH.h"
#include "Primitives.h"

namespace Primitives
{

    static float cubeVertices[] = {
        // positions          // normals           // texcoords
        // Back face
        -1.0f, -1.0f, -1.0f,   0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
         1.0f,  1.0f, -1.0f,   0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
         1.0f, -1.0f, -1.0f,   0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f, -1.0f,   0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        -1.0f, -1.0f, -1.0f,   0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
        -1.0f,  1.0f, -1.0f,   0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
        // Front face
        -1.0f, -1.0f,  1.0f,   0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f,   0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f,   0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
         1.0f,  1.0f,  1.0f,   0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
        -1.0f,  1.0f,  1.0f,   0.0f,  0.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  1.0f,   0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
        // Left face
        -1.0f,  1.0f,  1.0f,  -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        -1.0f,  1.0f, -1.0f,  -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        -1.0f, -1.0f, -1.0f,  -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -1.0f, -1.0f, -1.0f,  -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  1.0f,  -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        -1.0f,  1.0f,  1.0f,  -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        // Right face
         1.0f,  1.0f,  1.0f,   1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
         1.0f, -1.0f, -1.0f,   1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         1.0f,  1.0f, -1.0f,   1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
         1.0f, -1.0f, -1.0f,   1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         1.0f,  1.0f,  1.0f,   1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
         1.0f, -1.0f,  1.0f,   1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
         // Bottom face
         -1.0f, -1.0f, -1.0f,   0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
          1.0f, -1.0f, -1.0f,   0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
          1.0f, -1.0f,  1.0f,   0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
          1.0f, -1.0f,  1.0f,   0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
         -1.0f, -1.0f,  1.0f,   0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
         -1.0f, -1.0f, -1.0f,   0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
         // Top face
         -1.0f,  1.0f, -1.0f,   0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
          1.0f,  1.0f,  1.0f,   0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
          1.0f,  1.0f, -1.0f,   0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
          1.0f,  1.0f,  1.0f,   0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
         -1.0f,  1.0f, -1.0f,   0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
         -1.0f,  1.0f,  1.0f,   0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
    };

    Mesh CreateCube()
    {
        return CreateMesh(cubeVertices, 36, sizeof(float) * 8);
    }

    Mesh CreatePlane(float size)
    {
        float halfSize = size * 0.5f;
        float planeVertices[] = {
            -halfSize, 0.0f, -halfSize,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
             halfSize, 0.0f, -halfSize,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f,
             halfSize, 0.0f,  halfSize,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f,
             halfSize, 0.0f,  halfSize,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f,
            -halfSize, 0.0f,  halfSize,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f,
            -halfSize, 0.0f, -halfSize,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
        };
        return CreateMesh(planeVertices, 6, sizeof(float) * 8);
    }

    Mesh CreateSphere(int segments, int rings)
    {
        std::vector<float> vertices;

        for (int ring = 0; ring <= rings; ++ring)
        {
            float phi = glm::pi<float>() * ring / rings;
            for (int seg = 0; seg <= segments; ++seg)
            {
                float theta = 2.0f * glm::pi<float>() * seg / segments;

                float x = sin(phi) * cos(theta);
                float y = cos(phi);
                float z = sin(phi) * sin(theta);

                // Position
                vertices.push_back(x);
                vertices.push_back(y);
                vertices.push_back(z);
                // Normal (same as position for unit sphere)
                vertices.push_back(x);
                vertices.push_back(y);
                vertices.push_back(z);
                // UV
                vertices.push_back(static_cast<float>(seg) / segments);
                vertices.push_back(static_cast<float>(ring) / rings);
            }
        }

        // Build triangle list from the grid
        std::vector<float> triangleVertices;
        int vertsPerRow = segments + 1;

        for (int ring = 0; ring < rings; ++ring)
        {
            for (int seg = 0; seg < segments; ++seg)
            {
                int current = ring * vertsPerRow + seg;
                int next = current + vertsPerRow;

                auto addVertex = [&](int idx) {
                    for (int i = 0; i < 8; ++i)
                        triangleVertices.push_back(vertices[idx * 8 + i]);
                    };

                // First triangle
                addVertex(current);
                addVertex(next);
                addVertex(current + 1);

                // Second triangle
                addVertex(current + 1);
                addVertex(next);
                addVertex(next + 1);
            }
        }

        return CreateMesh(triangleVertices.data(), triangleVertices.size() / 8, sizeof(float) * 8);
    }

    Mesh CreateCylinder(float radius, float height, int segments)
    {
        // Implementation left as exercise - similar pattern to sphere
        return CreateCube(); // Placeholder
    }

} // namespace Primitives