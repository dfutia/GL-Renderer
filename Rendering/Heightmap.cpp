#include "PCH.h"
#include "Heightmap.h"
#include "Platform/File.h"

#include <stb_image.h>

Heightmap::Heightmap(const std::string& filepath)
{
    *this = LoadHeightmap(filepath);
}

Heightmap::Heightmap(int width, int height, float defaultValue)
    : width(width), height(height), data(width* height, defaultValue)
{
}

float Heightmap::Sample(float u, float v) const
{
    if (data.empty()) return 0.0f;

    // Wrap UVs
    u = u - floor(u);
    v = v - floor(v);

    int x = static_cast<int>(u * (width - 1));
    int y = static_cast<int>((1.0f - v) * (height - 1)); // flip Y

    return GetPixel(x, y);
}

float Heightmap::SampleBilinear(float u, float v) const
{
    if (data.empty()) return 0.0f;

    u = u - floor(u);
    v = 1.0f - (v - floor(v)); // flip Y

    float fx = u * (width - 1);
    float fy = v * (height - 1);

    int x0 = static_cast<int>(fx);
    int y0 = static_cast<int>(fy);
    int x1 = glm::min(x0 + 1, width - 1);
    int y1 = glm::min(y0 + 1, height - 1);

    float tx = fx - x0;
    float ty = fy - y0;

    float h00 = GetPixel(x0, y0);
    float h10 = GetPixel(x1, y0);
    float h01 = GetPixel(x0, y1);
    float h11 = GetPixel(x1, y1);

    float h0 = glm::mix(h00, h10, tx);
    float h1 = glm::mix(h01, h11, tx);

    return glm::mix(h0, h1, ty);
}

float Heightmap::GetPixel(int x, int y) const
{
    x = glm::clamp(x, 0, width - 1);
    y = glm::clamp(y, 0, height - 1);
    return data[y * width + x];
}

void Heightmap::SetPixel(int x, int y, float value)
{
    if (x >= 0 && x < width && y >= 0 && y < height)
        data[y * width + x] = value;
}

Heightmap LoadHeightmap(const std::string& filepath)
{
    std::vector<std::uint8_t> fileData = ReadBinaryFile(filepath);
    if (fileData.empty())
        throw std::runtime_error("Failed to load heightmap: " + filepath);

    int width, height, channels;
    unsigned char* pixels = stbi_load_from_memory(
        fileData.data(), static_cast<int>(fileData.size()),
        &width, &height, &channels, 1); // force single channel

    if (!pixels)
        throw std::runtime_error("Failed to decode heightmap: " + filepath);

    Heightmap map(width, height);

    for (int i = 0; i < width * height; i++)
        map.data[i] = pixels[i] / 255.0f;

    stbi_image_free(pixels);

    std::println("Loaded heightmap: {}x{}", width, height);
    return map;
}