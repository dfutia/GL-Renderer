#ifndef GAME_H
#define GAME_H

#include <memory>
#include <vector>
#include <print>

#include <glm/glm.hpp>
#include <nlohmann/json.hpp>

#include "Platform/File.h"
#include "Rendering/Mesh.h"
#include "Rendering/Material.h"
#include "Rendering/Heightmap.h"
#include "Rendering/LineRenderer.h"
#include "Graphics/VertexArray.h"
#include "Graphics/VertexBuffer.h"
#include "Actors/Actor.h"
#include "Actors/TransformComponent.h"
#include "Actors/ModelComponent.h"
#include "Actors/RenderComponent.h"

#include "BorderRenderer.h"
#include "GameManager.h"
#include "GeoData.h"

using json = nlohmann::json;

struct Coordinate
{
    float latitude;   // -90 to 90 (south to north)
    float longitude;  // -180 to 180 (west to east)
};

//inline glm::vec3 LatLonToPoint(float latitude, float longitude)
//{
//    float latRad = glm::radians(latitude);
//    float lonRad = glm::radians(longitude);
//
//    return glm::vec3(
//        cos(latRad) * sin(lonRad),
//        sin(latRad),
//        cos(latRad) * cos(lonRad)
//    );
//}
//
//inline glm::vec2 PointToLatLon(const glm::vec3& p)
//{
//    float lat = glm::degrees(asin(p.y));
//    float lon = glm::degrees(atan2(p.x, p.z));
//    return glm::vec2(lat, lon);
//}

Coordinate PointToCoordinate(const glm::vec3& pointOnUnitSphere)
{
    Coordinate coord;
    coord.latitude = glm::degrees(asin(pointOnUnitSphere.y));
    coord.longitude = glm::degrees(atan2(pointOnUnitSphere.x, pointOnUnitSphere.z));
    return coord;
}

glm::vec3 CoordinateToPoint(const Coordinate& coord)
{
    float latRad = glm::radians(coord.latitude);
    float lonRad = glm::radians(coord.longitude);

    glm::vec3 point;
    point.y = sin(latRad);
    point.x = cos(latRad) * sin(lonRad);
    point.z = cos(latRad) * cos(lonRad);

    return point;
}

glm::vec3 CoordinateToPoint(float latitude, float longitude)
{
    return CoordinateToPoint({ latitude, longitude });
}

glm::vec2 CoordinateToUV(const Coordinate& coord)
{
    float u = (coord.longitude + 180.0f) / 360.0f;
    float v = (coord.latitude + 90.0f) / 180.0f;
    return glm::vec2(u, v);
}

float SampleHeight(const glm::vec3& pointOnUnitSphere, const unsigned char* heightmapData, int width, int height)
{
    Coordinate coord = PointToCoordinate(pointOnUnitSphere);
    glm::vec2 uv = CoordinateToUV(coord);

    int x = static_cast<int>(uv.x * (width - 1));
    int y = static_cast<int>((1.0f - uv.y) * (height - 1)); // flip Y for image coordinates

    x = glm::clamp(x, 0, width - 1);
    y = glm::clamp(y, 0, height - 1);

    // Assuming grayscale heightmap, one byte per pixel
    unsigned char heightValue = heightmapData[y * width + x];
    return heightValue / 255.0f; // normalize to 0-1
}

glm::vec3 PointOnCubeToPointOnSphere(const glm::vec3& p)
{
    float x2 = p.x * p.x;
    float y2 = p.y * p.y;
    float z2 = p.z * p.z;

    float x = p.x * sqrt(1.0f - y2 / 2.0f - z2 / 2.0f + y2 * z2 / 3.0f);
    float y = p.y * sqrt(1.0f - z2 / 2.0f - x2 / 2.0f + z2 * x2 / 3.0f);
    float z = p.z * sqrt(1.0f - x2 / 2.0f - y2 / 2.0f + x2 * y2 / 3.0f);

    return glm::vec3(x, y, z);
}

std::vector<Vertex> CreateFace(const glm::vec3& normal, int resolution)
{
    std::vector<Vertex> vertices;

    glm::vec3 axisA = glm::vec3(normal.y, normal.z, normal.x);
    glm::vec3 axisB = glm::cross(normal, axisA);

    for (int y = 0; y < resolution; y++)
    {
        for (int x = 0; x < resolution; x++)
        {
            glm::vec2 percent = glm::vec2(x, y) / (float)(resolution - 1);
            glm::vec3 pointOnUnitCube = normal + (percent.x - 0.5f) * 2.0f * axisA + (percent.y - 0.5f) * 2.0f * axisB;
            glm::vec3 pointOnUnitSphere = PointOnCubeToPointOnSphere(pointOnUnitCube);

            Vertex vertex;
            vertex.position = pointOnUnitSphere;
            vertex.normal = glm::normalize(pointOnUnitSphere);
            vertex.texCoords = percent;
            vertex.tangent = glm::normalize(axisA);

            vertices.push_back(vertex);
        }
    }

    return vertices;
}

Mesh GenerateFaces(int resolution)
{
    std::vector<glm::vec3> directions = {
        glm::vec3(0, 1, 0),
        glm::vec3(0, -1, 0),
        glm::vec3(-1, 0, 0),
        glm::vec3(1, 0, 0),
        glm::vec3(0, 0, 1),
        glm::vec3(0, 0, -1)
    };

    std::vector<Vertex> allVertices;

    for (const auto& dir : directions)
    {
        auto faceVertices = CreateFace(dir, resolution);
        allVertices.insert(allVertices.end(), faceVertices.begin(), faceVertices.end());
    }

    // Generate indices for triangles
    std::vector<unsigned int> indices;
    int numFaces = 6;

    for (int face = 0; face < numFaces; face++)
    {
        int offset = face * resolution * resolution;

        for (int y = 0; y < resolution - 1; y++)
        {
            for (int x = 0; x < resolution - 1; x++)
            {
                int i = offset + y * resolution + x;

                // First triangle
                indices.push_back(i);
                indices.push_back(i + resolution);
                indices.push_back(i + resolution + 1);

                // Second triangle
                indices.push_back(i);
                indices.push_back(i + resolution + 1);
                indices.push_back(i + 1);
            }
        }
    }

    // Pack into buffer
    VertexDataBuffer buffer;
    for (const auto& v : allVertices)
    {
        buffer.Vec3(v.position);
        buffer.Vec3(v.normal);
        buffer.Vec2(v.texCoords);
        buffer.Vec3(v.tangent);
    }

    Mesh mesh;
    mesh.vertices = allVertices;
    mesh.indices = indices;
    mesh.vbo = new VertexBuffer(buffer.Pointer(), buffer.Size(), VertexBuffer::StaticDraw);
    mesh.vao = new VertexArray();

    unsigned int stride = sizeof(float) * 11;
    mesh.vao->BindAttribute(0, *mesh.vbo, GL_FLOAT, 3, stride, 0);
    mesh.vao->BindAttribute(1, *mesh.vbo, GL_FLOAT, 3, stride, sizeof(float) * 3);
    mesh.vao->BindAttribute(2, *mesh.vbo, GL_FLOAT, 2, stride, sizeof(float) * 6);
    mesh.vao->BindAttribute(3, *mesh.vbo, GL_FLOAT, 3, stride, sizeof(float) * 8);

    // Bind index buffer
    VertexBuffer* ebo = new VertexBuffer();
    ebo->Data(indices.data(), indices.size() * sizeof(unsigned int), VertexBuffer::StaticDraw);
    mesh.vao->BindElemenets(*ebo);

    return mesh;
}

class EarthComponent : public ActorComponent
{
public:
    float heightMultiplier = 19.97f;
    float seaLevel = 0.5f;
    bool useSeaLevel = true;
    glm::vec3 landColor = glm::vec3(0.2f, 0.6f, 0.1f);   // Green
    glm::vec3 seaColor = glm::vec3(0.1f, 0.3f, 0.8f);    // Blue
    glm::vec3 mountainColor = glm::vec3(0.5f, 0.4f, 0.3f); // Brown
    glm::vec3 snowColor = glm::vec3(1.0f, 1.0f, 1.0f);   // White
    float snowHeight = 0.8f;
    float mountainHeight = 0.6f;
    int resolution = 400;

    Heightmap* heightmap = nullptr;

    const char* GetName() const override { return "Earth"; }

    void OnUpdate(float deltaTime) override
    {
        // Sync uniforms to RenderComponent every frame
        if (auto* render = owner->GetComponent<RenderComponent>())
        {
            render->SetUniform("heightMultiplier", heightMultiplier);
            render->SetUniform("seaLevel", seaLevel);
            render->SetUniform("useSeaLevel", useSeaLevel ? 1 : 0);
            render->SetUniform("seaColor", seaColor);
            render->SetUniform("landColor", landColor);
            render->SetUniform("mountainColor", mountainColor);
            render->SetUniform("snowColor", snowColor);
            render->SetUniform("mountainHeight", mountainHeight);
            render->SetUniform("snowHeight", snowHeight);
        }
    }

    void RegisterProperties(PropertyRegistry& registry) override
    {
        registry.Float("Height Multiplier", &heightMultiplier, 0.0f, 50.0f, 0.1f);
        registry.Float("Sea Level", &seaLevel, 0.0f, 1.0f, 0.01f);
        registry.Bool("Use Sea Level", &useSeaLevel);
        registry.Color3("Sea Color", &seaColor);
        registry.Color3("Land Color", &landColor);
        registry.Color3("Mountain Color", &mountainColor);
        registry.Color3("Snow Color", &snowColor);
        registry.Float("Mountain Height", &mountainHeight, 0.0f, 1.0f, 0.01f);
        registry.Float("Snow Height", &snowHeight, 0.0f, 1.0f, 0.01f);
        registry.Int("Resolution", &resolution, 10, 200);
    }
};

//class BorderComponent : public ActorComponent
//{
//public:
//    float heightOffset = 0.002f;
//
//    void LoadBorders(const std::vector<CountryBorder>& countries)
//    {
//        lines.Clear();
//
//        for (const auto& country : countries)
//        {
//            for (const auto& polygon : country.polygons)
//            {
//                std::vector<glm::vec3> elevated;
//                elevated.reserve(polygon.size());
//
//                for (const auto& p : polygon)
//                    elevated.push_back(p * (1.0f + heightOffset));
//
//                lines.AddLineStrip(elevated, true);  // true = closed loop
//            }
//        }
//    }
//
//    const char* GetName() const override { return "Borders"; }
//
//    void RegisterProperties(PropertyRegistry& registry) override
//    {
//        registry.Color3("Color", &lines.color);
//        registry.Float("Line Width", &lines.lineWidth, 0.5f, 5.0f, 0.1f);
//        registry.Float("Height Offset", &heightOffset, 0.0f, 0.01f, 0.0001f);
//    }
//
//    LineRenderer lines;
//};

void GameMain(std::vector<std::unique_ptr<Actor>>& actors)
{
    // Load heightmap as a TEXTURE, not a Heightmap object
    Texture heightmapTexture = LoadTexture((GetMediaPath() / "Images/heightmap.png").string());

    auto earth = std::make_unique<Actor>();

    auto* earthComp = earth->AddComponent<EarthComponent>();

    // Simple unit sphere mesh - no heightmap needed
    Mesh earthMesh = GenerateFaces(earthComp->resolution);

    Material earthMaterial = Material::CreateDefault();
    earthMaterial.SetTexture(Material::Height, heightmapTexture);  // Slot 3 is Height

    auto* transform = earth->AddComponent<TransformComponent>();
    transform->position = glm::vec3(0.0f, 0.0f, 0.0f);
    transform->scale = glm::vec3(100.0f);

    earth->AddComponent<ModelComponent>(earthMesh, earthMaterial);

    auto* render = earth->AddComponent<RenderComponent>();
    render->shader = "earth";
    render->SetUniform("heightMultiplier", earthComp->heightMultiplier);
    render->castsShadows = false;

    static std::vector<CountryBorder> borders = LoadBordersSmart(
        (GetMediaPath() / "Data/countries.json").string()
    );
    auto* borderRenderer = earth->AddComponent<BorderRenderer>();
    borderRenderer->LoadBorders(borders);

    static std::vector<City> cities = LoadCitiesSmart(
        (GetMediaPath() / "Data/cities.json").string()
    );

    auto* gameManager = earth->AddComponent<GameManager>();
    gameManager->Initialize(cities);
    gameManager->earthRadius = 100.0f;  // Match your earth scale

    actors.push_back(std::move(earth));
}


#endif 