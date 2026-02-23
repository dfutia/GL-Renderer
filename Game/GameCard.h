#ifndef GAME_CARD_H
#define GAME_CARD_H

#include <vector>
#include <string>
#include <random>
#include "GeoData.h"

struct Destination
{
    std::string cityName;
    std::string countryName;
    glm::vec3 position;
    bool visited = false;
};

struct PlayerCard
{
    std::vector<Destination> destinations;
    int currentDestinationIndex = 0;

    bool AllVisited() const
    {
        for (const auto& dest : destinations)
        {
            if (!dest.visited)
                return false;
        }
        return true;
    }

    Destination* GetCurrentDestination()
    {
        if (currentDestinationIndex < destinations.size())
            return &destinations[currentDestinationIndex];
        return nullptr;
    }
};

class CardGenerator
{
public:
    void Initialize(const std::vector<City>& allCities, float minPopulation = 0.0f)
    {
        eligibleCities = allCities;
        std::println("Card generator initialized with {} eligible cities", eligibleCities.size());
    }

    PlayerCard GenerateCard(int numDestinations = 6)
    {
        PlayerCard card;

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dist(0, eligibleCities.size() - 1);

        std::set<int> usedIndices;

        while (card.destinations.size() < numDestinations)
        {
            int index = dist(gen);

            // Avoid duplicates
            if (usedIndices.count(index))
                continue;

            usedIndices.insert(index);

            const City& city = eligibleCities[index];

            Destination dest;
            dest.cityName = city.name;
            dest.countryName = city.country;
            dest.position = city.position;
            dest.visited = false;

            card.destinations.push_back(dest);
        }

        // Add New York as final destination
        Destination newYork;
        newYork.cityName = "New York";
        newYork.countryName = "United States";
        newYork.position = LatLonToPoint(40.7128f, -74.0060f);
        newYork.visited = false;
        card.destinations.push_back(newYork);

        return card;
    }

private:
    std::vector<City> eligibleCities;
};

#endif