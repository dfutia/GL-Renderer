#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H

#include "Actors/ActorComponent.h"
#include "Rendering/Camera.h"
#include "DestinationMarker.h"
#include "GeoData.h"
#include "GameCard.h"
#include "MousePicker.h"

class GameManager : public ActorComponent
{
public:
    PlayerCard playerCard;
    CardGenerator cardGenerator;

    const City* hoveredCity = nullptr;
    const City* selectedCity = nullptr;

    std::vector<City>* cities = nullptr;
    float earthRadius = 100.0f;

    Camera* camera = nullptr;
    DestinationMarker* markers = nullptr;

    void Initialize(std::vector<City>& cityData)
    {
        cities = &cityData;
        cardGenerator.Initialize(cityData, 500000.0f);
        playerCard = cardGenerator.GenerateCard(6);

        markers = owner->AddComponent<DestinationMarker>();
        markers->earthRadius = earthRadius;
        markers->Initialize();
        markers->SetDestinations(&playerCard.destinations, &playerCard.currentDestinationIndex);

        std::println("=== YOUR DESTINATIONS ===");
        for (int i = 0; i < playerCard.destinations.size(); i++)
        {
            const auto& dest = playerCard.destinations[i];
            std::println("{}. {}, {}", i + 1, dest.cityName, dest.countryName);
        }
        std::println("=========================");
    }

    void UpdateHover(int mouseX, int mouseY, int screenW, int screenH,
        const glm::mat4& view, const glm::mat4& projection,
        const glm::vec3& cameraPos)
    {
        if (!cities)
            return;

        glm::vec3 rayDir = MousePicker::GetRayFromScreen(
            mouseX, mouseY, screenW, screenH, view, projection);

        glm::vec3 hitPoint;
        if (MousePicker::RaySphereIntersect(cameraPos, rayDir, glm::vec3(0), earthRadius, hitPoint))
        {
            // Convert to unit sphere for city lookup
            glm::vec3 unitPoint = glm::normalize(hitPoint);
            hoveredCity = MousePicker::FindClosestCity(unitPoint, *cities, 0.05f);
        }
        else
        {
            hoveredCity = nullptr;
        }
    }

    void FocusOnDestination(int index)
    {
        if (index < 0 || index >= playerCard.destinations.size())
            return;

        if (!camera)
            return;

        const auto& dest = playerCard.destinations[index];

        // Position camera outside the sphere
        glm::vec3 dirFromCenter = glm::normalize(dest.position);
        float cameraDistance = earthRadius * 2.5f;
        camera->position = dirFromCenter * cameraDistance;

        // Look at the center of the earth
        camera->LookAt(glm::vec3(0.0f));
    }

    void OnClick()
    {
        if (hoveredCity)
        {
            selectedCity = hoveredCity;
            std::println("Selected: {}, {}", selectedCity->name, selectedCity->country);

            // Check if this is our current destination
            TryVisitCity(selectedCity->name);
        }
    }

    void TryVisitCity(const std::string& cityName)
    {
        auto* currentDest = playerCard.GetCurrentDestination();
        if (currentDest && currentDest->cityName == cityName)
        {
            currentDest->visited = true;
            playerCard.currentDestinationIndex++;

            std::println("*** VISITED: {} ***", cityName);

            if (playerCard.AllVisited())
            {
                std::println("!!! YOU WIN !!!");
            }
            else
            {
                auto* next = playerCard.GetCurrentDestination();
                if (next)
                    std::println("Next destination: {}, {}", next->cityName, next->countryName);
            }
        }
    }

    const char* GetName() const override { return "Game Manager"; }
};

#endif