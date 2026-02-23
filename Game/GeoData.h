#ifndef GEO_DATA_H
#define GEO_DATA_H

#include <vector>
#include <string>
#include <glm/glm.hpp>

struct CountryBorder
{
    std::string name;
    std::vector<std::vector<glm::vec3>> polygons;  // Multiple polygons per country (islands, etc.)
};

struct City
{
    std::string name;
    std::string country;
    glm::vec3 position;
    float population;
};

// Convert lat/lon to point on unit sphere
inline glm::vec3 LatLonToPoint(float latitude, float longitude)
{
    float latRad = glm::radians(latitude);
    float lonRad = glm::radians(longitude);

    // This must match the shader: lon = atan(x, z)
    // So x = sin(lon), z = cos(lon)
    return glm::vec3(
        cos(latRad) * sin(lonRad),   // X
        sin(latRad),                  // Y
        cos(latRad) * cos(lonRad)    // Z
    );
}

std::vector<CountryBorder> LoadCountryBorders(const std::string& geojsonPath);
std::vector<City> LoadCities(const std::string& geojsonPath);

void SaveCitiesBinary(const std::vector<City>& cities, const std::string& path);
std::vector<City> LoadCitiesBinary(const std::string& path);
std::vector<City> LoadCitiesSmart(const std::string& jsonPath);

void SaveBordersBinary(const std::vector<CountryBorder>& borders, const std::string& path);
std::vector<CountryBorder> LoadBordersBinary(const std::string& path);
std::vector<CountryBorder> LoadBordersSmart(const std::string& jsonPath);

#endif