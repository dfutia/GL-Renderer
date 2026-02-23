#ifndef PROPERTY_SYSTEM_H
#define PROPERTY_SYSTEM_H

#include <string>
#include <vector>
#include <variant>
#include <functional>
#include <glm/glm.hpp>

enum class PropertyType
{
    Int,
    Float,
    Bool,
    String,
    Vec2,
    Vec3,
    Vec4,
    Color3,
    Color4
};

struct Property
{
    std::string name;
    PropertyType type;
    void* data;
    float min = 0.0f;
    float max = 1.0f;
    float speed = 0.1f;
};

class PropertyRegistry
{
public:
    void Int(const std::string& name, int* value, int min = 0, int max = 100)
    {
        properties.push_back({ name, PropertyType::Int, value, (float)min, (float)max, 1.0f });
    }

    void Float(const std::string& name, float* value, float min = 0.0f, float max = 1.0f, float speed = 0.01f)
    {
        properties.push_back({ name, PropertyType::Float, value, min, max, speed });
    }

    void Bool(const std::string& name, bool* value)
    {
        properties.push_back({ name, PropertyType::Bool, value });
    }

    void String(const std::string& name, std::string* value)
    {
        properties.push_back({ name, PropertyType::String, value });
    }

    void Vec2(const std::string& name, glm::vec2* value, float min = -1000.0f, float max = 1000.0f, float speed = 0.1f)
    {
        properties.push_back({ name, PropertyType::Vec2, value, min, max, speed });
    }

    void Vec3(const std::string& name, glm::vec3* value, float min = -1000.0f, float max = 1000.0f, float speed = 0.1f)
    {
        properties.push_back({ name, PropertyType::Vec3, value, min, max, speed });
    }

    void Vec4(const std::string& name, glm::vec4* value, float min = -1000.0f, float max = 1000.0f, float speed = 0.1f)
    {
        properties.push_back({ name, PropertyType::Vec4, value, min, max, speed });
    }

    void Color3(const std::string& name, glm::vec3* value)
    {
        properties.push_back({ name, PropertyType::Color3, value });
    }

    void Color4(const std::string& name, glm::vec4* value)
    {
        properties.push_back({ name, PropertyType::Color4, value });
    }

    const std::vector<Property>& GetProperties() const { return properties; }
    void Clear() { properties.clear(); }

private:
    std::vector<Property> properties;
};

#endif