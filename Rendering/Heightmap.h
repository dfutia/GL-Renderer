#ifndef HEIGHTMAP_H
#define HEIGHTMAP_H

#include <string>
#include <vector>
#include <glm/glm.hpp>

class Heightmap
{
public:
    Heightmap() = default;
    Heightmap(const std::string& filepath);
    Heightmap(int width, int height, float defaultValue = 0.0f);
    ~Heightmap() = default;

    // Sample height at UV coordinates (0-1 range), returns 0-1
    float Sample(float u, float v) const;
    float Sample(const glm::vec2& uv) const { return Sample(uv.x, uv.y); }

    // Sample with bilinear interpolation for smoother results
    float SampleBilinear(float u, float v) const;
    float SampleBilinear(const glm::vec2& uv) const { return SampleBilinear(uv.x, uv.y); }

    // Direct pixel access
    float GetPixel(int x, int y) const;
    void SetPixel(int x, int y, float value);

    int GetWidth() const { return width; }
    int GetHeight() const { return height; }
    bool IsValid() const { return !data.empty(); }

    const float* Data() const { return data.data(); }

private:
    std::vector<float> data;
    int width = 0;
    int height = 0;

    friend Heightmap LoadHeightmap(const std::string& filepath);
};

Heightmap LoadHeightmap(const std::string& filepath);

#endif