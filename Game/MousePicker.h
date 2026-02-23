#ifndef MOUSE_PICKER_H
#define MOUSE_PICKER_H

#include <glm/glm.hpp>
#include "GeoData.h"

class MousePicker
{
public:
    // Convert screen coordinates to ray in world space
    static glm::vec3 GetRayFromScreen(
        int mouseX, int mouseY,
        int screenWidth, int screenHeight,
        const glm::mat4& view,
        const glm::mat4& projection)
    {
        // Convert to normalized device coordinates (-1 to 1)
        float x = (2.0f * mouseX) / screenWidth - 1.0f;
        float y = 1.0f - (2.0f * mouseY) / screenHeight;

        // Clip space
        glm::vec4 rayClip(x, y, -1.0f, 1.0f);

        // Eye space
        glm::vec4 rayEye = glm::inverse(projection) * rayClip;
        rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);

        // World space
        glm::vec3 rayWorld = glm::vec3(glm::inverse(view) * rayEye);
        return glm::normalize(rayWorld);
    }

    // Ray-sphere intersection
    // Returns true if hit, outputs hit point
    static bool RaySphereIntersect(
        const glm::vec3& rayOrigin,
        const glm::vec3& rayDir,
        const glm::vec3& sphereCenter,
        float sphereRadius,
        glm::vec3& hitPoint)
    {
        glm::vec3 oc = rayOrigin - sphereCenter;

        float a = glm::dot(rayDir, rayDir);
        float b = 2.0f * glm::dot(oc, rayDir);
        float c = glm::dot(oc, oc) - sphereRadius * sphereRadius;
        float discriminant = b * b - 4 * a * c;

        if (discriminant < 0)
            return false;

        float t = (-b - sqrt(discriminant)) / (2.0f * a);

        if (t < 0)
            t = (-b + sqrt(discriminant)) / (2.0f * a);

        if (t < 0)
            return false;

        hitPoint = rayOrigin + rayDir * t;
        return true;
    }

    // Find closest city to a point on the sphere
    static const City* FindClosestCity(
        const glm::vec3& pointOnSphere,
        const std::vector<City>& cities,
        float maxDistance = 0.1f)
    {
        const City* closest = nullptr;
        float closestDist = maxDistance;

        glm::vec3 normalizedPoint = glm::normalize(pointOnSphere);

        for (const auto& city : cities)
        {
            float dist = glm::distance(normalizedPoint, city.position);
            if (dist < closestDist)
            {
                closestDist = dist;
                closest = &city;
            }
        }

        return closest;
    }
};

#endif