#include "GeoData.h"
#include "Platform/File.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

std::vector<CountryBorder> LoadCountryBorders(const std::string& geojsonPath)
{
    std::vector<CountryBorder> borders;

    std::string content = ReadTextFile(geojsonPath);

    if (content.empty())
    {
        std::println("ERROR: Could not read file: {}", geojsonPath);
        return borders;
    }

    std::println("File size: {} bytes", content.size());

    json data = json::parse(content);

    std::string type = data.value("type", "");
    std::println("JSON type: {}", type);

    auto parsePolygon = [](const json& coords) -> std::vector<glm::vec3>
        {
            std::vector<glm::vec3> points;
            for (const auto& coord : coords)
            {
                float lon = coord[0];
                float lat = coord[1];
                points.push_back(LatLonToPoint(lat, lon));
            }
            return points;
        };

    auto parseGeometry = [&](const json& geometry) -> std::vector<std::vector<glm::vec3>>
        {
            std::vector<std::vector<glm::vec3>> polygons;

            std::string geomType = geometry["type"];

            if (geomType == "Polygon")
            {
                for (const auto& ring : geometry["coordinates"])
                {
                    polygons.push_back(parsePolygon(ring));
                }
            }
            else if (geomType == "MultiPolygon")
            {
                for (const auto& polygon : geometry["coordinates"])
                {
                    for (const auto& ring : polygon)
                    {
                        polygons.push_back(parsePolygon(ring));
                    }
                }
            }

            return polygons;
        };

    // Handle FeatureCollection (standard GeoJSON)
    if (type == "FeatureCollection" && data.contains("features"))
    {
        std::println("Parsing FeatureCollection with {} features", data["features"].size());

        for (const auto& feature : data["features"])
        {
            CountryBorder country;

            // Try various property names for country name
            if (feature.contains("properties"))
            {
                const auto& props = feature["properties"];
                if (props.contains("ADMIN"))
                    country.name = props["ADMIN"];
                else if (props.contains("name"))
                    country.name = props["name"];
                else if (props.contains("NAME"))
                    country.name = props["NAME"];
                else if (props.contains("NAME_EN"))
                    country.name = props["NAME_EN"];
            }

            if (feature.contains("geometry"))
            {
                country.polygons = parseGeometry(feature["geometry"]);
            }

            if (!country.polygons.empty())
                borders.push_back(country);
        }
    }
    // Handle GeometryCollection (what you have)
    else if (type == "GeometryCollection" && data.contains("geometries"))
    {
        std::println("Parsing GeometryCollection with {} geometries", data["geometries"].size());

        int index = 0;
        for (const auto& geometry : data["geometries"])
        {
            CountryBorder country;
            country.name = "Country_" + std::to_string(index++);  // No names available
            country.polygons = parseGeometry(geometry);

            if (!country.polygons.empty())
                borders.push_back(country);
        }
    }
    else
    {
        std::println("ERROR: Unknown JSON structure. Type: {}", type);
    }

    std::println("Loaded {} countries", borders.size());
    return borders;
}

std::vector<City> LoadCities(const std::string& path)
{
    std::vector<City> cities;

    std::string content = ReadTextFile(path);
    if (content.empty())
    {
        std::println("ERROR: Could not read cities file: {}", path);
        return cities;
    }

    std::println("Cities file size: {} bytes", content.size());

    json data;
    try
    {
        data = json::parse(content);
    }
    catch (const json::parse_error& e)
    {
        std::println("JSON PARSE ERROR: {}", e.what());
        return cities;
    }

    // Handle plain array format (lutangar style)
    if (data.is_array())
    {
        std::println("Parsing cities array with {} entries", data.size());

        for (const auto& item : data)
        {
            try
            {
                City city;

                // Name
                if (item.contains("name") && item["name"].is_string())
                    city.name = item["name"];
                else
                    continue;

                // Country
                if (item.contains("country") && item["country"].is_string())
                    city.country = item["country"];
                else
                    city.country = "Unknown";

                // Coordinates - handle both string and number formats
                float lat = 0, lon = 0;

                if (item.contains("lat"))
                {
                    if (item["lat"].is_string())
                        lat = std::stof(item["lat"].get<std::string>());
                    else if (item["lat"].is_number())
                        lat = item["lat"].get<float>();
                }

                if (item.contains("lng"))
                {
                    if (item["lng"].is_string())
                        lon = std::stof(item["lng"].get<std::string>());
                    else if (item["lng"].is_number())
                        lon = item["lng"].get<float>();
                }

                city.position = LatLonToPoint(lat, lon);
                city.population = 1;

                cities.push_back(city);
            }
            catch (...)
            {
                // Skip bad entries
            }
        }
    }
    // Handle GeoJSON FeatureCollection format
    else if (data.contains("features"))
    {
        std::println("Parsing GeoJSON FeatureCollection...");
        // ... existing GeoJSON parsing code ...
    }
    else
    {
        std::println("ERROR: Unknown JSON format - not an array or FeatureCollection");
    }

    std::println("Loaded {} cities", cities.size());
    return cities;
}

void SaveCitiesBinary(const std::vector<City>& cities, const std::string& path)
{
    std::ofstream file(path, std::ios::binary);
    if (!file) return;

    uint32_t count = cities.size();
    file.write(reinterpret_cast<char*>(&count), sizeof(count));

    for (const auto& city : cities)
    {
        uint32_t nameLen = city.name.size();
        file.write(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
        file.write(city.name.data(), nameLen);

        uint32_t countryLen = city.country.size();
        file.write(reinterpret_cast<char*>(&countryLen), sizeof(countryLen));
        file.write(city.country.data(), countryLen);

        file.write(reinterpret_cast<const char*>(&city.position), sizeof(glm::vec3));
        file.write(reinterpret_cast<const char*>(&city.population), sizeof(float));
    }

    std::println("Saved {} cities to cache", count);
}

std::vector<City> LoadCitiesBinary(const std::string& path)
{
    std::vector<City> cities;
    std::ifstream file(path, std::ios::binary);
    if (!file) return cities;

    uint32_t count;
    file.read(reinterpret_cast<char*>(&count), sizeof(count));
    cities.reserve(count);

    for (uint32_t i = 0; i < count; i++)
    {
        City city;

        uint32_t nameLen;
        file.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
        city.name.resize(nameLen);
        file.read(city.name.data(), nameLen);

        uint32_t countryLen;
        file.read(reinterpret_cast<char*>(&countryLen), sizeof(countryLen));
        city.country.resize(countryLen);
        file.read(city.country.data(), countryLen);

        file.read(reinterpret_cast<char*>(&city.position), sizeof(glm::vec3));
        file.read(reinterpret_cast<char*>(&city.population), sizeof(float));

        cities.push_back(city);
    }

    std::println("Loaded {} cities from cache", cities.size());
    return cities;
}

std::vector<City> LoadCitiesSmart(const std::string& jsonPath)
{
    std::string cachePath = jsonPath + ".cache";

    // Check if cache exists
    std::ifstream cacheTest(cachePath, std::ios::binary);
    if (cacheTest.good())
    {
        cacheTest.close();
        return LoadCitiesBinary(cachePath);
    }

    // Parse JSON (slow, but only once)
    std::println("First run - parsing JSON (this will be cached)...");
    auto cities = LoadCities(jsonPath);

    // Save cache for next time
    if (!cities.empty())
        SaveCitiesBinary(cities, cachePath);

    return cities;
}

void SaveBordersBinary(const std::vector<CountryBorder>& borders, const std::string& path)
{
    std::ofstream file(path, std::ios::binary);
    if (!file) return;

    uint32_t countryCount = borders.size();
    file.write(reinterpret_cast<char*>(&countryCount), sizeof(countryCount));

    for (const auto& country : borders)
    {
        // Write name
        uint32_t nameLen = country.name.size();
        file.write(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
        file.write(country.name.data(), nameLen);

        // Write polygon count
        uint32_t polyCount = country.polygons.size();
        file.write(reinterpret_cast<char*>(&polyCount), sizeof(polyCount));

        for (const auto& polygon : country.polygons)
        {
            // Write vertex count
            uint32_t vertCount = polygon.size();
            file.write(reinterpret_cast<char*>(&vertCount), sizeof(vertCount));

            // Write all vertices at once
            file.write(reinterpret_cast<const char*>(polygon.data()), vertCount * sizeof(glm::vec3));
        }
    }

    std::println("Saved {} countries to cache", countryCount);
}

std::vector<CountryBorder> LoadBordersBinary(const std::string& path)
{
    std::vector<CountryBorder> borders;
    std::ifstream file(path, std::ios::binary);
    if (!file) return borders;

    uint32_t countryCount;
    file.read(reinterpret_cast<char*>(&countryCount), sizeof(countryCount));
    borders.reserve(countryCount);

    for (uint32_t i = 0; i < countryCount; i++)
    {
        CountryBorder country;

        // Read name
        uint32_t nameLen;
        file.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
        country.name.resize(nameLen);
        file.read(country.name.data(), nameLen);

        // Read polygon count
        uint32_t polyCount;
        file.read(reinterpret_cast<char*>(&polyCount), sizeof(polyCount));
        country.polygons.resize(polyCount);

        for (uint32_t p = 0; p < polyCount; p++)
        {
            // Read vertex count
            uint32_t vertCount;
            file.read(reinterpret_cast<char*>(&vertCount), sizeof(vertCount));

            // Read all vertices at once
            country.polygons[p].resize(vertCount);
            file.read(reinterpret_cast<char*>(country.polygons[p].data()), vertCount * sizeof(glm::vec3));
        }

        borders.push_back(country);
    }

    std::println("Loaded {} countries from cache", borders.size());
    return borders;
}

std::vector<CountryBorder> LoadBordersSmart(const std::string& jsonPath)
{
    std::string cachePath = jsonPath + ".cache";

    std::ifstream cacheTest(cachePath, std::ios::binary);
    if (cacheTest.good())
    {
        cacheTest.close();
        return LoadBordersBinary(cachePath);
    }

    std::println("First run - parsing borders JSON (this will be cached)...");
    auto borders = LoadCountryBorders(jsonPath);

    if (!borders.empty())
        SaveBordersBinary(borders, cachePath);

    return borders;
}