#ifndef DESTINATION_MARKER_H
#define DESTINATION_MARKER_H

#include "Actors/ActorComponent.h"
#include "Graphics/VertexArray.h"
#include "Graphics/VertexBuffer.h"
#include "GameCard.h"
#include <vector>

class GraphicsDevice;

class DestinationMarker : public ActorComponent
{
public:
    float markerSize = 1.5f;
    float heightAboveSurface = 2.0f;
    float earthRadius = 100.0f;

    glm::vec3 unvisitedColor = glm::vec3(1.0f, 0.2f, 0.2f);  // Red
    glm::vec3 currentColor = glm::vec3(1.0f, 1.0f, 0.0f);    // Yellow
    glm::vec3 visitedColor = glm::vec3(0.2f, 1.0f, 0.2f);    // Green

    void Initialize();
    void SetDestinations(const std::vector<Destination>* dests, int* currentIndex);
    void Render(GraphicsDevice& graphics, const glm::mat4& view, const glm::mat4& proj);

    const char* GetName() const override { return "Destination Marker"; }

    void RegisterProperties(PropertyRegistry& registry) override
    {
        registry.Float("Marker Size", &markerSize, 0.5f, 10.0f, 0.1f);
        registry.Float("Height Above Surface", &heightAboveSurface, 0.0f, 20.0f, 0.5f);
        registry.Color3("Unvisited Color", &unvisitedColor);
        registry.Color3("Current Color", &currentColor);
        registry.Color3("Visited Color", &visitedColor);
    }

private:
    const std::vector<Destination>* destinations = nullptr;
    int* currentDestinationIndex = nullptr;

    VertexArray* sphereVAO = nullptr;
    VertexBuffer* sphereVBO = nullptr;
    VertexBuffer* sphereEBO = nullptr;
    int indexCount = 0;

    void CreateSphereMesh(int stacks, int slices);
};

#endif