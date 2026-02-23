#include "DestinationMarker.h"
#include "Graphics/GraphicsDevice.h"
#include "Actors/TransformComponent.h"

void DestinationMarker::Initialize()
{
    CreateSphereMesh(12, 12);
}

void DestinationMarker::SetDestinations(const std::vector<Destination>* dests, int* currentIndex)
{
    destinations = dests;
    currentDestinationIndex = currentIndex;
}

void DestinationMarker::CreateSphereMesh(int stacks, int slices)
{
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    for (int i = 0; i <= stacks; i++)
    {
        float phi = glm::pi<float>() * float(i) / float(stacks);

        for (int j = 0; j <= slices; j++)
        {
            float theta = 2.0f * glm::pi<float>() * float(j) / float(slices);

            float x = sin(phi) * cos(theta);
            float y = cos(phi);
            float z = sin(phi) * sin(theta);

            // Position
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
        }
    }

    for (int i = 0; i < stacks; i++)
    {
        for (int j = 0; j < slices; j++)
        {
            int first = i * (slices + 1) + j;
            int second = first + slices + 1;

            indices.push_back(first);
            indices.push_back(second);
            indices.push_back(first + 1);

            indices.push_back(second);
            indices.push_back(second + 1);
            indices.push_back(first + 1);
        }
    }

    indexCount = indices.size();

    sphereVBO = new VertexBuffer(vertices.data(), vertices.size() * sizeof(float), VertexBuffer::StaticDraw);
    sphereVAO = new VertexArray();
    sphereVAO->BindAttribute(0, *sphereVBO, GL_FLOAT, 3, sizeof(float) * 3, 0);

    sphereEBO = new VertexBuffer();
    sphereEBO->Data(indices.data(), indices.size() * sizeof(unsigned int), VertexBuffer::StaticDraw);
    sphereVAO->BindElemenets(*sphereEBO);
}

void DestinationMarker::Render(GraphicsDevice& graphics, const glm::mat4& view, const glm::mat4& proj)
{
    if (!destinations || !sphereVAO)
        return;

    graphics.SetUniform("viewMatrix", view);
    graphics.SetUniform("projectionMatrix", proj);

    for (size_t i = 0; i < destinations->size(); i++)
    {
        const auto& dest = (*destinations)[i];

        // Position marker above the surface
        glm::vec3 dir = glm::normalize(dest.position);
        glm::vec3 markerPos = dir * (earthRadius + heightAboveSurface);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, markerPos);
        model = glm::scale(model, glm::vec3(markerSize));

        // Choose color based on state
        glm::vec3 color;
        if (dest.visited)
            color = visitedColor;
        else if (currentDestinationIndex && (int)i == *currentDestinationIndex)
            color = currentColor;
        else
            color = unvisitedColor;

        graphics.SetUniform("modelMatrix", model);
        graphics.SetUniform("color", glm::vec4(color, 1.0f));

        graphics.BindResource(ResourceType::VERTEX_BUFFER, *sphereVAO);
        graphics.DrawIndexed(indexCount);
    }
}